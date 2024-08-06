#include "taskwidgetoverlay.h"

TaskWidgetOverlay::TaskWidgetOverlay(QWidget *parent) : QWidget(parent) {
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setAutoFillBackground(false);
}

void TaskWidgetOverlay::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::TextAntialiasing);

  QPixmap dropIcon(":/resources/icons/drag-drop-line.png");
  QString textLabel(tr("Drag and drop Images here"));

  int iconSize = 64;
  int textHeight = 30;
  int spacing = 10;

  int totalHeight = iconSize + textHeight + spacing;
  int yOffset = (height() - totalHeight) / 2;

  QRect iconRect((width() - iconSize) / 2, yOffset, iconSize, iconSize);
  QRect textRect(0, yOffset + iconSize + spacing, width(), textHeight);

  painter.drawPixmap(iconRect, dropIcon);

  painter.setPen(QColor(Qt::gray));
  painter.drawText(textRect, Qt::AlignCenter, textLabel);
}
