#ifndef ELIDEDITEMDELEGATE_H
#define ELIDEDITEMDELEGATE_H

#include <QPainter>
#include <QStyledItemDelegate>
#include <QApplication>

class ElidedItemDelegate : public QStyledItemDelegate {
  Q_OBJECT

public:
  explicit ElidedItemDelegate(QObject *parent = nullptr);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;
};

#endif // ELIDEDITEMDELEGATE_H
