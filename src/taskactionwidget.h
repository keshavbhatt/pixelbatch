#ifndef TASKACTIONWIDGET_H
#define TASKACTIONWIDGET_H

#include "taskwidget.h"

#include <QWidget>

class TaskWidget;

namespace Ui {
class TaskActionWidget;
}

class TaskActionWidget : public QWidget {
  Q_OBJECT

public:
  explicit TaskActionWidget(QWidget *parent = nullptr,
                            TaskWidget *taskWidget = nullptr);
  ~TaskActionWidget();

public slots:
  void updateActions();

private:
  Ui::TaskActionWidget *ui;

  TaskWidget *m_taskWidget;
};

#endif // TASKACTIONWIDGET_H
