#ifndef TASKACTIONWIDGET_H
#define TASKACTIONWIDGET_H

#include <QTableWidget>
#include <QWidget>

namespace Ui {
class TaskActionWidget;
}

class TaskActionWidget : public QWidget {
  Q_OBJECT

public:
  explicit TaskActionWidget(QWidget *parent = nullptr);
  ~TaskActionWidget();

public slots:
  void updateActions(QTableWidget *taskWidget);

private:
  Ui::TaskActionWidget *ui;
};

#endif // TASKACTIONWIDGET_H
