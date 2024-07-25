#include "taskwidget.h"

#include "elideditemdelegate.h"
#include "imagestats.h"
#include "imagetask.h"
#include "settings.h"

#include <worker/ImageWorker.h>
#include <worker/imageworkerfactory.h>

#include <QDir>
#include <QMainWindow>

TaskWidget::TaskWidget(QWidget *parent)
    : QTableWidget(parent), m_settings(Settings::instance()) {
  setAcceptDrops(true);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setMouseTracking(true);
  setEditTriggers(QAbstractItemView::NoEditTriggers);

  updateTaskWidgetHeader(false);

  connect(this->model(), &QAbstractItemModel::rowsInserted, this, &TaskWidget::updateStatusMessage);
  connect(this->model(), &QAbstractItemModel::rowsRemoved, this, &TaskWidget::updateStatusMessage);

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
  // Get file size
  QFileInfo fileInfo(filePath);
  qint64 fileSize = fileInfo.size();

  // Create a new row
  int newRow = rowCount();
  insertRow(newRow);

  // Insert file path
  QTableWidgetItem *fileItem = new QTableWidgetItem(filePath);
  setItem(newRow, 0, fileItem);

  // Insert initial status
  QTableWidgetItem *statusItem = new QTableWidgetItem("Pending");
  statusItem->setTextAlignment(Qt::AlignCenter);
  setItem(newRow, 1, statusItem);

  // Insert file size before
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

  QString destination = m_settings.getOptimizedPath() + m_settings.getOutputFilePrefix() + fileInfo.fileName();
  m_imageTasks.append(ImageTask(filePath, destination, newRow));

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
    for (const ImageTask &imageTask : qAsConst(m_imageTasks)) {
        try {
            // Ensure destination dir exists
            QFileInfo destinationInfo(imageTask.optimizedPath);
            QDir().mkpath(destinationInfo.absoluteDir().absolutePath());

            // Process
            ImageWorker *worker = ImageWorkerFactory::getWorker(imageTask.imagePath);
            connect(worker, &ImageWorker::optimizationFinished, this, [this, worker](const ImageTask &task,const bool success) {
                onOptimizationFinished(task, success);
                worker->deleteLater();
            });
            connect(worker, &ImageWorker::optimizationError, this, [this, worker](const ImageTask &task, const QString &errorString) {
                onOptimizationError(task, errorString);
                worker->deleteLater();
            });
            worker->optimize(imageTask);
        } catch (const std::exception &e) {
            // TODO: update imageTask and corresponding row
            qDebug() << "Error processing " << imageTask.imagePath << ": " << e.what();
        }
    }
}

void TaskWidget::updateTaskStatus(const ImageTask &task, const QString text,
                                  const QString tooltip) {
  int row = task.rowIndex;
  QTableWidgetItem *statusItem = item(row, 1);

  statusItem->setToolTip(tooltip);
  statusItem->setText(text);
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

void TaskWidget::onOptimizationFinished(const ImageTask &task, bool success) {
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
    updateTaskStatus(task, "Success");
  } else {
    updateTaskStatus(task, "Falied");
  }
}

void TaskWidget::onOptimizationError(const ImageTask &task,
                                     const QString &errorString) {
  updateTaskStatus(task, "Falied", errorString);
}
