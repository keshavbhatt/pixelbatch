#ifndef TASKWIDGETOVERLAY_H
#define TASKWIDGETOVERLAY_H

#include <QPaintEvent>
#include <QPainter>
#include <QWidget>

class TaskWidgetOverlay : public QWidget {
public:
  explicit TaskWidgetOverlay(QWidget *parent = nullptr);

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif // TASKWIDGETOVERLAY_H
