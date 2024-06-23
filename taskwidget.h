#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include <QDragEnterEvent>
#include <QFileInfo>
#include <QImageReader>
#include <QMimeData>
#include <QTableWidget>

class TaskWidget : public QTableWidget {

  Q_OBJECT

public:
  explicit TaskWidget(QWidget *parent = nullptr);

signals:
  void setStatusRequested(const QString &message);

protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dropEvent(QDropEvent *event) override;
};

#endif // TASKWIDGET_H
