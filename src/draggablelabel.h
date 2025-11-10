#ifndef DRAGGABLELABEL_H
#define DRAGGABLELABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QPoint>
#include <QScrollArea>

class DraggableLabel : public QLabel {
  Q_OBJECT

public:
  explicit DraggableLabel(QWidget *parent = nullptr);
  ~DraggableLabel();

  void setScrollArea(QScrollArea *scrollArea);

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

private:
  bool m_isDragging;
  QPoint m_lastMousePos;
  QScrollArea *m_scrollArea;
};

#endif // DRAGGABLELABEL_H

