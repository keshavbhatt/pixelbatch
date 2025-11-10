#include "draggablelabel.h"

#include <QScrollBar>

DraggableLabel::DraggableLabel(QWidget *parent)
    : QLabel(parent), m_isDragging(false), m_scrollArea(nullptr) {
  setCursor(Qt::OpenHandCursor);
  // Enable mouse tracking for better cursor feedback
  setMouseTracking(true);
}

DraggableLabel::~DraggableLabel() {}

void DraggableLabel::setScrollArea(QScrollArea *scrollArea) {
  m_scrollArea = scrollArea;
}

void DraggableLabel::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton && m_scrollArea) {
    m_isDragging = true;
    m_lastMousePos = event->globalPos(); // Use globalPos for more accurate tracking
    setCursor(Qt::ClosedHandCursor);
    event->accept();
  } else {
    QLabel::mousePressEvent(event);
  }
}

void DraggableLabel::mouseMoveEvent(QMouseEvent *event) {
  if (m_isDragging && m_scrollArea) {
    QPoint delta = m_lastMousePos - event->globalPos();
    m_lastMousePos = event->globalPos();

    // Update scroll bars with direct value setting for smoother performance
    QScrollBar *hBar = m_scrollArea->horizontalScrollBar();
    QScrollBar *vBar = m_scrollArea->verticalScrollBar();

    if (hBar && delta.x() != 0) {
      int newValue = hBar->value() + delta.x();
      hBar->setValue(qBound(hBar->minimum(), newValue, hBar->maximum()));
    }
    if (vBar && delta.y() != 0) {
      int newValue = vBar->value() + delta.y();
      vBar->setValue(qBound(vBar->minimum(), newValue, vBar->maximum()));
    }

    event->accept();
  } else {
    QLabel::mouseMoveEvent(event);
  }
}

void DraggableLabel::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton && m_isDragging) {
    m_isDragging = false;
    setCursor(Qt::OpenHandCursor);
    event->accept();
  } else {
    QLabel::mouseReleaseEvent(event);
  }
}

