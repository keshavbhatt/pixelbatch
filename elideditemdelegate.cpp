#include "elideditemdelegate.h"

ElidedItemDelegate::ElidedItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

void ElidedItemDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const {
  QStyleOptionViewItem opt = option;
  opt.textElideMode = Qt::ElideMiddle;
  QStyledItemDelegate::paint(painter, opt, index);
}
