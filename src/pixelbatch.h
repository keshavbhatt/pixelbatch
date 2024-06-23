#ifndef PIXELBATCH_H
#define PIXELBATCH_H

#include "taskwidget.h"

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class PixelBatch;
}
QT_END_NAMESPACE

class PixelBatch : public QMainWindow {
  Q_OBJECT

public:
  PixelBatch(QWidget *parent = nullptr);
  ~PixelBatch();

private slots:
  void setStatus(const QString &message);

  void toggleShowStatusBarAddButton(const bool visible);

private:
  Ui::PixelBatch *ui;

  TaskWidget *m_taskWidget;
  QPushButton *m_statusBarAddButton;
  QLabel *m_permanentStatusbarMessageLabel;

  void initTaskWidget();
  void setupStatusBar();
};
#endif // PIXELBATCH_H
