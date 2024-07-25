#ifndef PIXELBATCH_H
#define PIXELBATCH_H

#include "filehandler.h"
#include "taskwidget.h"

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>

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

  // actions
  void addImages();
  void openSettings();
  void quitApplication();
  void removeFinishedOperations();
  void clearAllOperations();
  void reportIssue();
  void donate();
  void showAbout();

private:
  Ui::PixelBatch *ui;

  TaskWidget *m_taskWidget;
  QWidget *m_statusBarPermanentWidget;
  QPushButton *m_statusBarAddButton;
  QPushButton *m_statusBarProcessButton;
  QLabel *m_StatusbarPermanentMessageLabel;
  FileHandler *m_fileHandler;
  Settings &m_settings;


  void initTaskWidget();
  void setupStatusBar();
  void initMenuBar();
};
#endif // PIXELBATCH_H
