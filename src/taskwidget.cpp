#include "taskwidget.h"

#include "elideditemdelegate.h"
#include "imagestats.h"
#include "imagetask.h"

#include <worker/ImageWorker.h>
#include <worker/imageworkerfactory.h>

TaskWidget::TaskWidget(QWidget *parent) : QTableWidget(parent) {
  setAcceptDrops(true);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setMouseTracking(true);

  updateTaskWidgetHeader(false);
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

  // Insert initial status
  QTableWidgetItem *statusItem = new QTableWidgetItem("Pending");
  setItem(newRow, 0, statusItem);

  // Insert file path
  QTableWidgetItem *fileItem = new QTableWidgetItem(filePath);
  setItem(newRow, 1, fileItem);

  // Insert file size
  QString formattedSize = m_locale.formattedDataSize(fileSize);
  QTableWidgetItem *sizeItem = new QTableWidgetItem(formattedSize);
  setItem(newRow, 2, sizeItem);

  // Placeholder for savings column (you can replace this with actual logic)
  QTableWidgetItem *savingsItem = new QTableWidgetItem("N/A");
  setItem(newRow, 3, savingsItem);

  // Map file name to row index
  fileRowMap[filePath] = newRow;

  images.push_back(filePath);

  updateTaskWidgetHeader(true);
}

void TaskWidget::updateTaskWidgetHeader(const bool &contentLoaded) {
  if (!contentLoaded) {
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  } else {
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
  }
}

void TaskWidget::processImages() {
    for (const auto &imagePath : images) {
        QFileInfo fileInfo(imagePath);
        QString dst = "/tmp/d/optimized_" + fileInfo.fileName();
        ImageTask task(imagePath, dst, fileRowMap[imagePath]);
        ImageWorker* worker = nullptr;
        try {
            worker = ImageWorkerFactory::getWorker(imagePath);
            connect(worker, &ImageWorker::optimizationFinished, this, &TaskWidget::onOptimizationFinished);
            connect(worker, &ImageWorker::optimizationError, this, &TaskWidget::onOptimizationError);
            worker->optimize(task);
        } catch (const std::exception &e) {
            qDebug() << "Error processing " << imagePath << ": " << e.what();
            delete worker; // Clean up the worker if an exception is thrown
        }
    }
}

void TaskWidget::onOptimizationFinished(const ImageTask &task, bool success) {
    int row = task.rowIndex;
    QTableWidgetItem *statusItem = item(row, 0);
    QTableWidgetItem *savingsItem = item(row, 3);

    if (success) {
        statusItem->setText("Success");

        ImageStats stats(task.imagePath, task.optimizedPath); // Adjust based on your ImageStats implementation

        qint64 savings = stats.getSavings();
        double compressionPercentage = stats.getCompressionPercentage();

        QString formattedSavings = m_locale.formattedDataSize(savings);

        savingsItem->setText(QString("%1 (%2%)")
                                 .arg(formattedSavings, QString::number(compressionPercentage, 'f', 2)));
    } else {
        statusItem->setText("Failed");
    }
}

void TaskWidget::onOptimizationError(const ImageTask &task, const QString &errorString) {
    int row = task.rowIndex;
    QTableWidgetItem *statusItem = item(row, 0);
    statusItem->setText("Error: " + errorString);
}
