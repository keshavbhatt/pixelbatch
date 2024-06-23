#include "taskwidget.h"

#include "elideditemdelegate.h"

TaskWidget::TaskWidget(QWidget *parent) : QTableWidget(parent) {
  setAcceptDrops(true);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setMouseTracking(true);
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
      if (QImageReader::supportedImageFormats().contains(
              fileInfo.suffix().toLower().toUtf8())) {
        // Handle the dropped image file here
        // For example, add it to the table or perform other actions
        // You can get the file path using fileInfo.absoluteFilePath()

        // Example: Adding file name to the table
        int row = rowCount();
        insertRow(row);
        setItem(row, 1, new QTableWidgetItem(fileInfo.fileName()));
        setItem(row, 2, new QTableWidgetItem(QString::number(fileInfo.size())));
        setItem(row, 3,
                new QTableWidgetItem("0%")); // Placeholder for "Savings" column
      }
    }
    event->acceptProposedAction();
  }
}
