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
    : QTableWidget(parent), m_settings(Settings::instance()) {
  setAcceptDrops(true);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setMouseTracking(true);
  setEditTriggers(QAbstractItemView::NoEditTriggers);

  updateTaskWidgetHeader(false);

  connect(this->model(), &QAbstractItemModel::rowsInserted, this,
          &TaskWidget::updateStatusMessage);
  connect(this->model(), &QAbstractItemModel::rowsRemoved, this,
          &TaskWidget::updateStatusMessage);
}

void TaskWidget::updateStatusMessage() {
  int rowCount = this->rowCount();
  QString message = tr("Loaded %1 items").arg(rowCount);

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

void TaskWidget::addFileToTable(const QString &filePath) {

  QFileInfo fileInfo(filePath);

  QString destination = m_settings.getOptimizedPath() +
                        m_settings.getOutputFilePrefix() + fileInfo.fileName();

  // Create a new row
  int newRow = rowCount();
  insertRow(newRow);

  // Insert ImageTask in m_imageTasks list
  ImageTask *imageTask = new ImageTask(filePath, destination, newRow);
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

  updateTaskWidgetHeader(true);
}

void TaskWidget::updateTaskWidgetHeader(const bool &contentLoaded) {
  if (!contentLoaded) {
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  } else {
    // horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    // horizontalHeader()->setStretchLastSection(true);
  }
}

void TaskWidget::processImages() {
  for (ImageTask *imageTask : qAsConst(m_imageTasks)) {
    m_imageTaskQueue.enqueue(imageTask);
  }

  processNextBatch();
}

void TaskWidget::processNextBatch() {
  while (m_activeTasks < maxConcurrentTasks && !m_imageTaskQueue.isEmpty()) {
    ImageTask *imageTask = m_imageTaskQueue.dequeue();
    m_activeTasks++;
    imageTask->taskStatus = ImageTask::Processing;

    try {
      // Ensure destination dir exists
      QFileInfo destinationInfo(imageTask->optimizedPath);
      QDir().mkpath(destinationInfo.absoluteDir().absolutePath());

      // Process
      ImageWorker *worker = ImageWorkerFactory::getWorker(imageTask->imagePath);
      connect(worker, &ImageWorker::optimizationFinished, this,
              [this, worker](ImageTask &task, bool success) {
                task.taskStatus =
                    success ? ImageTask::Completed : ImageTask::Error;
                onOptimizationFinished(task, success);
                worker->deleteLater();
                m_activeTasks--;
                processNextBatch();
              });
      connect(worker, &ImageWorker::optimizationError, this,
              [this, worker](ImageTask &task, const QString &errorString) {
                task.taskStatus = ImageTask::Error;
                onOptimizationError(task, errorString);
                worker->deleteLater();
                m_activeTasks--;
                processNextBatch();
              });
      worker->optimize(imageTask);
    } catch (const std::exception &e) {
      // TODO: update imageTask and corresponding row
      qDebug() << "Error processing " << imageTask->imagePath << ": "
               << e.what();
      m_activeTasks--;
      processNextBatch();
    }
  }
}

void TaskWidget::removeFinishedOperations() {
  QList<ImageTask *> tasksToRemove;

  for (ImageTask *imageTask : qAsConst(m_imageTasks)) {
    if (imageTask->taskStatus == ImageTask::Completed) {
      tasksToRemove.append(imageTask);
    }
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
  }
}

int TaskWidget::findRowByImageTask(ImageTask *task) {
  for (int row = 0; row < rowCount(); ++row) {
    QTableWidgetItem *item = this->item(row, 0); // data is in column 0
    if (item) {
      ImageTask *storedTask = item->data(Qt::UserRole).value<ImageTask *>();
      if (storedTask == task) {
        return row;
      }
    }
  }
  return -1;
}

void TaskWidget::updateTaskStatus(const ImageTask &task,
                                  const QString optionalDetail) {
  int row = task.rowIndex;
  QTableWidgetItem *statusItem = item(row, 1);

  statusItem->setToolTip(optionalDetail);
  statusItem->setText(task.statusToString());
}

void TaskWidget::updateTaskSizeAfter(const ImageTask &task,
                                     const QString text) {
  int row = task.rowIndex;
  QTableWidgetItem *sizeAfterItem = item(row, 3);

  sizeAfterItem->setText(text);
}

void TaskWidget::updateTaskSaving(const ImageTask &task, const QString text) {
  int row = task.rowIndex;
  QTableWidgetItem *savingItem = item(row, 4);

  savingItem->setText(text);
}

void TaskWidget::onOptimizationFinished(ImageTask &task, bool success) {
  qDebug() << "Optimization finished for" << task.imagePath
           << "with success:" << success;

  if (success) {
    ImageStats stats(task.imagePath, task.optimizedPath);

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

void TaskWidget::onOptimizationError(ImageTask &task,
                                     const QString &errorString) {
  qDebug() << "Optimization error for" << task.imagePath << ":" << errorString;
  updateTaskStatus(task, errorString);
}
