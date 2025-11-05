#include "taskwidget.h"

#include "elideditemdelegate.h"
#include "imagestats.h"
#include "imagetask.h"
#include "settings.h"

#include <worker/ImageWorker.h>
#include <worker/imageworkerfactory.h>

#include <QDir>
#include <QMainWindow>
#include <QQueue>
#include <QThreadPool>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QMessageBox>
#include <QRegularExpression>
#include <QKeyEvent>
#include <QMouseEvent>

TaskWidget::TaskWidget(QWidget *parent)
    : QTableWidget(parent), m_overlayWidget(new TaskWidgetOverlay(this)),
      m_settings(Settings::instance()) {

  m_overlayWidget->setGeometry(this->rect());
  updateTaskOverlayWidget();

  setAcceptDrops(true);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setMouseTracking(true);
  setEditTriggers(QAbstractItemView::NoEditTriggers);

  updateTableHeader(false);

  connect(this->model(), &QAbstractItemModel::rowsInserted, this, [=]() {
    // we are updating the status button and message in addFileToTable method
    // since the insertRow event is emited before actually adding the ImageTask
    // to the table
    updateTaskOverlayWidget();
  });

  connect(this->model(), &QAbstractItemModel::rowsRemoved, this, [=]() {
    updateStatusBarMessage(getSummaryAndUpdateView());
    updateTaskOverlayWidget();
    emit isProcessingChanged(false); // force update buttons
  });
}

TaskWidget::~TaskWidget() {
  qDebug() << "TaskWidget destructor: Cleaning up active workers and processes";
  cancelAllProcessing();

  // Clean up image tasks
  qDeleteAll(m_imageTasks);
  m_imageTasks.clear();
}

void TaskWidget::cancelAllProcessing() {
  qDebug() << "Cancelling all processing - " << m_activeWorkers.count() << " active workers";

  // Delete workers immediately (not deleteLater) to ensure cleanup happens NOW
  for (QObject *worker : m_activeWorkers) {
    if (worker) {
      delete worker;  // Immediate deletion triggers destructor NOW
    }
  }
  m_activeWorkers.clear();

  // Clear the queue
  m_imageTaskQueue.clear();
  m_activeTasks = 0;

  qDebug() << "All processing cancelled and workers cleaned up";
}

void TaskWidget::updateStatusBarMessage(const QString &message) {
  emit statusMessageUpdated(message);
}

void TaskWidget::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasUrls()) {
    bool hasValidImage = false;
    foreach (const QUrl &url, event->mimeData()->urls()) {
      QFileInfo fileInfo(url.toLocalFile());
      if (QImageReader::supportedImageFormats().contains(
              fileInfo.suffix().toLower().toUtf8())) {
        hasValidImage = true;
        break;
      }
    }

    if (hasValidImage) {
      event->acceptProposedAction();
      // Visual feedback: could add border highlight here if needed
      return;
    }
  }
  event->ignore();
}

void TaskWidget::dragMoveEvent(QDragMoveEvent *event) {
  event->acceptProposedAction();
}

void TaskWidget::dropEvent(QDropEvent *event) {
  if (event->mimeData()->hasUrls()) {
    QStringList addedFiles;
    QStringList skippedFiles;

    foreach (const QUrl &url, event->mimeData()->urls()) {
      QFileInfo fileInfo(url.toLocalFile());

      // Check if file is a supported image format
      if (!QImageReader::supportedImageFormats().contains(
              fileInfo.suffix().toLower().toUtf8())) {
        skippedFiles << fileInfo.fileName();
        continue;
      }

      // Check if file exists
      if (!fileInfo.exists()) {
        skippedFiles << fileInfo.fileName();
        continue;
      }

      this->addFileToTable(fileInfo.filePath());
      addedFiles << fileInfo.fileName();
    }

    // Provide feedback
    if (!addedFiles.isEmpty()) {
      updateStatusBarMessage(tr("Added %1 image(s)").arg(addedFiles.count()));
    }

    if (!skippedFiles.isEmpty() && skippedFiles.count() <= 5) {
      QMessageBox::information(this, tr("Files Skipped"),
                               tr("Skipped %1 unsupported or invalid file(s):\n%2")
                                   .arg(skippedFiles.count())
                                   .arg(skippedFiles.join("\n")));
    } else if (!skippedFiles.isEmpty()) {
      QMessageBox::information(this, tr("Files Skipped"),
                               tr("Skipped %1 unsupported or invalid file(s)")
                                   .arg(skippedFiles.count()));
    }

    event->acceptProposedAction();
  }
}

void TaskWidget::resizeEvent(QResizeEvent *event) {
  QTableWidget::resizeEvent(event);
  m_overlayWidget->setGeometry(this->rect());
}

void TaskWidget::selectionChanged(const QItemSelection &selected,
                                  const QItemSelection &deselected) {
  QTableWidget::selectionChanged(selected, deselected);
  emit selectionChangedCustom();
  emit toggleShowTaskActionWidget(selectedItems().count() > 0);
}

void TaskWidget::mousePressEvent(QMouseEvent *event) {
  // Get the item at the click position
  QTableWidgetItem *item = itemAt(event->pos());

  // If clicked on empty space (no item), clear selection
  if (!item) {
    clearSelection();
    event->accept();
    return;
  }

  // Otherwise, handle normally
  QTableWidget::mousePressEvent(event);
}

void TaskWidget::keyPressEvent(QKeyEvent *event) {
  // Delete key to remove selected task
  if (event->key() == Qt::Key_Delete && hasSelection()) {
    removeSelectedRow();
    event->accept();
    return;
  }

  // Enter/Return to open optimized image
  if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) && hasSelection()) {
    ImageTask *task = getImageTaskFromRow(currentRow());
    if (task && task->taskStatus == ImageTask::Completed) {
      openOptimizedImageInImageViewerForSelectedTask();
      event->accept();
      return;
    }
  }

  // Escape to deselect current selection
  if (event->key() == Qt::Key_Escape && hasSelection()) {
    clearSelection();
    event->accept();
    return;
  }

  // Ctrl+A to select all (default behavior, but we ensure it works)
  if (event->matches(QKeySequence::SelectAll)) {
    selectAll();
    event->accept();
    return;
  }

  QTableWidget::keyPressEvent(event);
}

void TaskWidget::addFileToTable(const QString &filePath) {

  QFileInfo fileInfo(filePath);

  // Check for duplicates
  for (const ImageTask *existingTask : qAsConst(m_imageTasks)) {
    if (existingTask->imagePath == filePath) {
      QMessageBox::information(this, tr("Duplicate File"),
                               tr("The file '%1' is already in the list.")
                                   .arg(fileInfo.fileName()));
      return;
    }
  }

  QString destination = m_settings.getOptimizedPath() +
                        m_settings.getOutputFilePrefix() + fileInfo.fileName();

  // Create a new row
  int newRow = rowCount();
  insertRow(newRow);

  // Insert ImageTask in m_imageTasks list
  ImageTask *imageTask = new ImageTask(filePath, destination);
  imageTask->taskStatus = ImageTask::Pending;
  m_imageTasks.append(imageTask);

  // Insert file path
  QTableWidgetItem *fileItem = new QTableWidgetItem(filePath);
  // we use this data to map ImageTask to table row
  fileItem->setData(Qt::UserRole, QVariant::fromValue<ImageTask *>(imageTask));
  fileItem->setToolTip(filePath); // Show full path on hover
  setItem(newRow, 0, fileItem);

  // Insert initial status
  QTableWidgetItem *statusItem =
      new QTableWidgetItem(imageTask->statusToString());
  statusItem->setTextAlignment(Qt::AlignCenter);
  statusItem->setToolTip(tr("Waiting to be processed"));
  setItem(newRow, 1, statusItem);

  // Insert file size before
  qint64 fileSize = fileInfo.size();
  QString formattedSize = m_locale.formattedDataSize(fileSize);
  QTableWidgetItem *sizeBeforeItem = new QTableWidgetItem(formattedSize);
  sizeBeforeItem->setTextAlignment(Qt::AlignCenter);
  sizeBeforeItem->setToolTip(tr("Original file size: %1 bytes").arg(fileSize));
  setItem(newRow, 2, sizeBeforeItem);

  // Insert file size after
  QTableWidgetItem *sizeAfterItem = new QTableWidgetItem("—");
  sizeAfterItem->setTextAlignment(Qt::AlignCenter);
  sizeAfterItem->setToolTip(tr("Not yet optimized"));
  setItem(newRow, 3, sizeAfterItem);

  // savings
  QTableWidgetItem *savingsItem = new QTableWidgetItem("—");
  savingsItem->setTextAlignment(Qt::AlignCenter);
  savingsItem->setToolTip(tr("Not yet optimized"));
  setItem(newRow, 4, savingsItem);

  updateStatusBarMessage(getSummaryAndUpdateView());

  emit isProcessingChanged(false); // force update buttons

  updateTableHeader(true);
}

void TaskWidget::updateTableHeader(const bool &contentLoaded) {
  if (!contentLoaded) {
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  } else {
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
  }
}

void TaskWidget::processImages() {
  int queuedCount = 0;

  for (ImageTask *imageTask : qAsConst(m_imageTasks)) {
    // Process pending tasks
    if (imageTask->taskStatus == ImageTask::Pending) {
      imageTask->taskStatus = ImageTask::Queued;
      m_imageTaskQueue.enqueue(imageTask);
      updateTaskStatus(imageTask);
      queuedCount++;
    }
    // Re-process completed or error tasks
    else if (imageTask->taskStatus == ImageTask::Completed ||
             imageTask->taskStatus == ImageTask::Error) {
      // Reset status to Queued for re-processing
      imageTask->taskStatus = ImageTask::Queued;
      m_imageTaskQueue.enqueue(imageTask);
      updateTaskStatus(imageTask);

      // Clear previous results
      updateTaskSizeAfter(imageTask, "—");
      updateTaskSaving(imageTask, "—");

      queuedCount++;
    }
  }

  if (queuedCount > 0) {
    setIsProcessing(true);
    updateStatusBarMessage(
        tr("%1 %2 queued for processing")
            .arg(QString::number(queuedCount),
                 QString(queuedCount > 1 ? "items" : "item")));
  }

  processNextBatch();
}

ImageTask::TaskStatusCounts TaskWidget::getTaskStatusCounts() const {
  ImageTask::TaskStatusCounts counts;

  for (const ImageTask *task : qAsConst(m_imageTasks)) {
    if (task->taskStatus == ImageTask::Completed) {
      counts.completedCount++;
    } else if (task->taskStatus == ImageTask::Error) {
      counts.errorCount++;
    } else if (task->taskStatus == ImageTask::Pending) {
      counts.pendingCount++;
    } else if (task->taskStatus == ImageTask::Queued) {
      counts.queuedCount++;
    }
  }

  counts.totalTasks = m_imageTasks.count();
  return counts;
}

bool TaskWidget::isProcessing() const { return m_isProcessing; }

QString
TaskWidget::generateSummary(const ImageTask::TaskStatusCounts &counts) const {
  QString summary =
      QString(tr("Pending") + ": %1, ").arg(counts.pendingCount) +
      QString(tr("Queued") + ": %1, ").arg(counts.queuedCount) +
      QString(tr("Success") + ": %1, ").arg(counts.completedCount) +
      QString(tr("Error") + ": %1, ").arg(counts.errorCount) +
      QString(tr("Total") + ": %1").arg(counts.totalTasks);

  return summary;
}

QString TaskWidget::getSummaryAndUpdateView() {
  auto taskStatusCounts = getTaskStatusCounts();
  return generateSummary(taskStatusCounts);
}

void TaskWidget::processNextBatch() {
  int maxConcurrentTasks = m_settings.getMaxConcurrentTasks();
  while (m_activeTasks < maxConcurrentTasks && !m_imageTaskQueue.isEmpty()) {
    ImageTask *imageTask = m_imageTaskQueue.dequeue();
    m_activeTasks++;
    imageTask->taskStatus = ImageTask::Processing;
    updateTaskStatus(imageTask);
    try {
      // Ensure destination dir exists
      QFileInfo destinationInfo(imageTask->optimizedPath);
      QDir().mkpath(destinationInfo.absoluteDir().absolutePath());

      // Process
      ImageWorker *worker =
          ImageWorkerFactory::instance().getWorker(imageTask->imagePath);

      // Track worker for cleanup
      m_activeWorkers.append(worker);

      connect(worker, &ImageWorker::optimizationFinished, this,
              [this, worker](ImageTask *task, bool success) {
                task->taskStatus =
                    success ? ImageTask::Completed : ImageTask::Error;
                onOptimizationFinished(task, success);
                m_activeWorkers.removeOne(worker);
                worker->deleteLater();
                m_activeTasks--;
                processNextBatch();
              });
      connect(worker, &ImageWorker::optimizationError, this,
              [this, worker](ImageTask *task, const QString &errorString) {
                task->taskStatus = ImageTask::Error;
                onOptimizationError(task, errorString);
                m_activeWorkers.removeOne(worker);
                worker->deleteLater();
                m_activeTasks--;
                processNextBatch();
              });
      worker->optimize(imageTask);
    } catch (const std::exception &e) {
      updateTaskStatus(imageTask, e.what());
      qWarning() << "Error processing " << imageTask->imagePath << ": "
                 << e.what();
      m_activeTasks--;
      processNextBatch();
    }

    // Update progress message
    int processedTasks = m_imageTasks.count() - m_imageTaskQueue.count() - m_activeTasks;
    int totalTasks = m_imageTasks.count();
    int remainingTasks = m_imageTaskQueue.count();

    updateStatusBarMessage(tr("Processing: %1 of %2 complete (%3 in progress, %4 remaining)")
                               .arg(processedTasks)
                               .arg(totalTasks)
                               .arg(m_activeTasks)
                               .arg(remainingTasks));
  }

  if (m_imageTaskQueue.isEmpty() && m_activeTasks == 0) {
    updateStatusBarMessage(getSummaryAndUpdateView());
    setIsProcessing(false);
  }
}

void TaskWidget::removeTasksByStatus(const ImageTask::Status &status) {
  QList<ImageTask *> tasksToRemove;

  for (ImageTask *imageTask : qAsConst(m_imageTasks)) {
    if (imageTask->taskStatus == status) {
      tasksToRemove.append(imageTask);
    }
  }

  for (ImageTask *task : tasksToRemove) {
    removeTask(task);
  }
}

void TaskWidget::removeFinishedOperations() {
  removeTasksByStatus(ImageTask::Completed);
}

void TaskWidget::clearAllOperations() {
  QList<ImageTask *> tasksToRemove;

  for (ImageTask *imageTask : qAsConst(m_imageTasks)) {
    tasksToRemove.append(imageTask);
  }

  for (ImageTask *task : tasksToRemove) {
    removeTask(task);
  }
}

void TaskWidget::removeTask(ImageTask *task) {
  int row = findRowByImageTask(task);
  if (row >= 0) {
    m_imageTaskQueue.removeAll(task);
    m_imageTasks.removeAll(task);

    this->removeRow(row);

    delete task;
  } else {
    qWarning() << "Unable to remove task, task not found in task table";
  }
}

void TaskWidget::removeSelectedRow() {

  ImageTask *task = getImageTaskFromRow(this->currentRow());

  if (task) {
    removeTask(task);
  }
}

void TaskWidget::openOptimizedImageInFileManagerForSelectedTask() {
  ImageTask *task = getImageTaskFromRow(this->currentRow());

  if (!task) {
    qWarning() << "No task selected";
    return;
  }

  // Check if the optimized file exists
  QFileInfo fileInfo(task->optimizedPath);
  if (!fileInfo.exists()) {
    QMessageBox::warning(this, tr("File Not Found"),
                         tr("The optimized image does not exist yet.\n"
                            "Please ensure the image has been processed."));
    return;
  }

  // Open the directory containing the file in the default file manager
  QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
}

void TaskWidget::openOptimizedImageInImageViewerForSelectedTask() {
  ImageTask *task = getImageTaskFromRow(this->currentRow());

  if (!task) {
    qWarning() << "No task selected";
    return;
  }

  // Check if the optimized file exists
  QFileInfo fileInfo(task->optimizedPath);
  if (!fileInfo.exists()) {
    QMessageBox::warning(this, tr("File Not Found"),
                         tr("The optimized image does not exist yet.\n"
                            "Please ensure the image has been processed."));
    return;
  }

  // Open the file with the system's default image viewer
  if (!QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()))) {
    QMessageBox::warning(this, tr("Error Opening File"),
                         tr("Failed to open the optimized image.\n"
                            "Please check that you have a default image viewer configured."));
  }
}

void TaskWidget::openOriginalImageInImageViewerForSelectedTask() {
  ImageTask *task = getImageTaskFromRow(this->currentRow());

  if (!task) {
    qWarning() << "No task selected";
    return;
  }

  // Check if the original file exists
  QFileInfo fileInfo(task->imagePath);
  if (!fileInfo.exists()) {
    QMessageBox::warning(this, tr("File Not Found"),
                         tr("The original image file does not exist.\n"
                            "It may have been moved or deleted."));
    return;
  }

  // Open the file with the system's default image viewer
  if (!QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()))) {
    QMessageBox::warning(this, tr("Error Opening File"),
                         tr("Failed to open the original image.\n"
                            "Please check that you have a default image viewer configured."));
  }
}

int TaskWidget::findRowByImageTask(ImageTask *task) {
  for (int row = 0; row < rowCount(); ++row) {
    // ImageTask(task) pointer is mapped with data of QTableWidgetItem in first
    // column
    QTableWidgetItem *item = this->item(row, 0);
    if (item) {
      ImageTask *storedTask = item->data(Qt::UserRole).value<ImageTask *>();
      if (storedTask == task) {
        return row;
      }
    }
  }
  return -1;
}

ImageTask *TaskWidget::getImageTaskFromRow(int row) {
  QTableWidgetItem *item = this->item(row, 0);
  if (item) {
    ImageTask *storedTask = item->data(Qt::UserRole).value<ImageTask *>();
    return storedTask;
  }
  return nullptr;
}

void TaskWidget::updateTaskStatus(ImageTask *task,
                                  const QString optionalDetail) {
  int row = findRowByImageTask(task);
  if (row >= 0) {
    QTableWidgetItem *statusItem = item(row, 1);

    // Set status text
    statusItem->setText(task->statusToString());

    // Set tooltip with details
    QString tooltip;
    switch (task->taskStatus) {
      case ImageTask::Pending:
        tooltip = tr("Waiting to be processed");
        statusItem->setForeground(QBrush(QColor("#999999"))); // Gray
        break;
      case ImageTask::Queued:
        tooltip = tr("Queued for processing");
        statusItem->setForeground(QBrush(QColor("#0066CC"))); // Blue
        break;
      case ImageTask::Processing:
        tooltip = tr("Currently being optimized...");
        statusItem->setForeground(QBrush(QColor("#FF8800"))); // Orange
        break;
      case ImageTask::Completed:
        tooltip = tr("Successfully optimized");
        statusItem->setForeground(QBrush(QColor("#00AA00"))); // Green
        break;
      case ImageTask::Error:
        tooltip = optionalDetail.isEmpty()
            ? tr("Optimization failed")
            : tr("Error: %1").arg(optionalDetail);
        statusItem->setForeground(QBrush(QColor("#CC0000"))); // Red
        break;
    }

    if (!optionalDetail.isEmpty() && task->taskStatus != ImageTask::Error) {
      tooltip += "\n" + optionalDetail;
    }

    statusItem->setToolTip(tooltip);
  }
}

void TaskWidget::updateTaskSizeAfter(ImageTask *task, const QString text) {
  int row = findRowByImageTask(task);
  if (row >= 0) {
    QTableWidgetItem *sizeAfterItem = item(row, 3);
    sizeAfterItem->setText(text);

    // Add tooltip with formatted size
    QFileInfo fileInfo(task->optimizedPath);
    if (fileInfo.exists()) {
      qint64 bytes = fileInfo.size();
      sizeAfterItem->setToolTip(tr("Optimized file size: %1 bytes").arg(bytes));
    }
  }
}

void TaskWidget::updateTaskSaving(ImageTask *task, const QString text) {
  int row = findRowByImageTask(task);
  if (row >= 0) {
    QTableWidgetItem *savingItem = item(row, 4);
    savingItem->setText(text);

    // Extract percentage from text (format: "XXX KB (YY.YY%)")
    QRegularExpression re("\\(([\\d.]+)%\\)");
    QRegularExpressionMatch match = re.match(text);

    if (match.hasMatch()) {
      double percentage = match.captured(1).toDouble();

      // Color code based on savings
      if (percentage >= 30.0) {
        savingItem->setForeground(QBrush(QColor("#00AA00"))); // Excellent - Dark Green
        savingItem->setToolTip(tr("Excellent compression! Saved %1%").arg(QString::number(percentage, 'f', 2)));
      } else if (percentage >= 15.0) {
        savingItem->setForeground(QBrush(QColor("#44AA44"))); // Good - Green
        savingItem->setToolTip(tr("Good compression. Saved %1%").arg(QString::number(percentage, 'f', 2)));
      } else if (percentage >= 5.0) {
        savingItem->setForeground(QBrush(QColor("#FF8800"))); // Moderate - Orange
        savingItem->setToolTip(tr("Moderate compression. Saved %1%").arg(QString::number(percentage, 'f', 2)));
      } else if (percentage > 0.0) {
        savingItem->setForeground(QBrush(QColor("#999999"))); // Minimal - Gray
        savingItem->setToolTip(tr("Minimal compression. Saved %1%").arg(QString::number(percentage, 'f', 2)));
      } else {
        savingItem->setForeground(QBrush(QColor("#666666"))); // No savings - Dark Gray
        savingItem->setToolTip(tr("No compression achieved"));
      }
    }
  }
}

void TaskWidget::onOptimizationFinished(ImageTask *task, bool success) {
  // qDebug() << "Optimization finished for" << task->imagePath
  //          << "with success:" << success;

  if (success) {
    ImageStats stats(task->imagePath, task->optimizedPath);

    // update saving
    qint64 savings = stats.getSavings();
    double compressionPercentage = stats.getCompressionPercentage();
    QString formattedSavings = m_locale.formattedDataSize(savings);
    updateTaskSaving(task,
                     QString("%1 (%2%)")
                         .arg(formattedSavings,
                              QString::number(compressionPercentage, 'f', 2)));

    // update sizeafter
    qint64 sizeAfter = stats.getOptimizedSize();
    QString formattedSizeAfter = m_locale.formattedDataSize(sizeAfter);
    updateTaskSizeAfter(task, formattedSizeAfter);

    // update status
    updateTaskStatus(task);
  } else {
    updateTaskStatus(task);
  }
}

void TaskWidget::onOptimizationError(ImageTask *task,
                                     const QString &errorString) {
  qWarning() << "Optimization error for" << task->imagePath << ":"
             << errorString;
  updateTaskStatus(task, errorString);
}

void TaskWidget::setIsProcessing(bool value) {
  if (m_isProcessing != value) {
    m_isProcessing = value;
    emit isProcessingChanged(m_isProcessing);
  }
}

void TaskWidget::updateTaskOverlayWidget() {
  rowCount() == 0 ? m_overlayWidget->show() : m_overlayWidget->hide();
}

bool TaskWidget::hasSelection() {
  QList<QTableWidgetItem *> selectedItems = this->selectedItems();
  return !selectedItems.isEmpty();
}
