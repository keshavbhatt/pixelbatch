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

void TaskWidget::updateStatusBarMessage(const QString &message) {
  emit statusMessageUpdated(message);
}

void TaskWidget::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasUrls()) {
    foreach (const QUrl &url, event->mimeData()->urls()) {
      QFileInfo fileInfo(url.toLocalFile());
      if (QImageReader::supportedImageFormats().contains(
              fileInfo.suffix().toLower().toUtf8())) {
        event->acceptProposedAction();
        return;
      }
    }
  }
  event->ignore();
}

void TaskWidget::dragMoveEvent(QDragMoveEvent *event) {
  event->acceptProposedAction();
}

void TaskWidget::dropEvent(QDropEvent *event) {
  if (event->mimeData()->hasUrls()) {
    foreach (const QUrl &url, event->mimeData()->urls()) {
      QFileInfo fileInfo(url.toLocalFile());
      this->addFileToTable(fileInfo.filePath());
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

void TaskWidget::addFileToTable(const QString &filePath) {

  QFileInfo fileInfo(filePath);

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
  setItem(newRow, 0, fileItem);

  // Insert initial status
  QTableWidgetItem *statusItem =
      new QTableWidgetItem(imageTask->statusToString());
  statusItem->setTextAlignment(Qt::AlignCenter);
  setItem(newRow, 1, statusItem);

  // Insert file size before
  qint64 fileSize = fileInfo.size();
  QString formattedSize = m_locale.formattedDataSize(fileSize);
  QTableWidgetItem *sizeBeforeItem = new QTableWidgetItem(formattedSize);
  sizeBeforeItem->setTextAlignment(Qt::AlignCenter);
  setItem(newRow, 2, sizeBeforeItem);

  // Insert file size after
  QTableWidgetItem *sizeAfterItem = new QTableWidgetItem("N/A");
  sizeAfterItem->setTextAlignment(Qt::AlignCenter);
  setItem(newRow, 3, sizeAfterItem);

  // savings
  QTableWidgetItem *savingsItem = new QTableWidgetItem("N/A");
  savingsItem->setTextAlignment(Qt::AlignCenter);
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
  for (ImageTask *imageTask : qAsConst(m_imageTasks)) {
    if (imageTask->taskStatus == ImageTask::Pending) {
      imageTask->taskStatus = ImageTask::Queued;
      m_imageTaskQueue.enqueue(imageTask);
      updateTaskStatus(imageTask);
    }
  }

  if (m_imageTasks.count() > 0) {
    setIsProcessing(true);
    updateStatusBarMessage(
        tr("%1 %2 queued")
            .arg(QString::number(m_imageTasks.count()),
                 QString(m_imageTasks.count() > 1 ? "item" : "items")));
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
      ImageWorker *worker = ImageWorkerFactory::getWorker(imageTask->imagePath);
      connect(worker, &ImageWorker::optimizationFinished, this,
              [this, worker](ImageTask *task, bool success) {
                task->taskStatus =
                    success ? ImageTask::Completed : ImageTask::Error;
                onOptimizationFinished(task, success);
                worker->deleteLater();
                m_activeTasks--;
                processNextBatch();
              });
      connect(worker, &ImageWorker::optimizationError, this,
              [this, worker](ImageTask *task, const QString &errorString) {
                task->taskStatus = ImageTask::Error;
                onOptimizationError(task, errorString);
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
    int processedTasks = m_imageTasks.count() - m_imageTaskQueue.count();
    updateStatusBarMessage(tr("Processed %1 of %2")
                               .arg(QString::number(processedTasks),
                                    QString::number(m_imageTasks.count())));
  }

  if (m_imageTaskQueue.isEmpty()) {
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
  Q_UNIMPLEMENTED();
}

void TaskWidget::openOptimizedImageInImageViewerForSelectedTask() {
  Q_UNIMPLEMENTED();
}

void TaskWidget::openOriginalImageInImageViewerForSelectedTask() {
  Q_UNIMPLEMENTED();
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

    statusItem->setToolTip(optionalDetail);
    statusItem->setText(task->statusToString());
  }
}

void TaskWidget::updateTaskSizeAfter(ImageTask *task, const QString text) {
  int row = findRowByImageTask(task);
  if (row >= 0) {
    QTableWidgetItem *sizeAfterItem = item(row, 3);

    sizeAfterItem->setText(text);
  }
}

void TaskWidget::updateTaskSaving(ImageTask *task, const QString text) {
  int row = findRowByImageTask(task);
  if (row >= 0) {
    QTableWidgetItem *savingItem = item(row, 4);

    savingItem->setText(text);
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
