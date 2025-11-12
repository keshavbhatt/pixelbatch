#ifndef EMPTYSTATEWIDGET_H
#define EMPTYSTATEWIDGET_H

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class EmptyStateWidget : public QWidget {
  Q_OBJECT

public:
  explicit EmptyStateWidget(QWidget *parent = nullptr);
  ~EmptyStateWidget();

private:
  void setupUI();
};

#endif // EMPTYSTATEWIDGET_H

