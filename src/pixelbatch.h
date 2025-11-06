#ifndef PIXELBATCH_H
#define PIXELBATCH_H

#include "filehandler.h"
#include "preferenceswidget.h"
#include "taskwidget.h"

#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
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

  void addFileFromCommandLine(const QString &filePath);

private slots:
  void setStatus(const QString &message);
  void updateStatusBarButtons(bool processing = false);
  void updateMenuActions(bool processing = false);

  // actions
  void addImages();
  void openSettings();
  void quitApplication();
  void reportIssue();
  void donate();
  void showAbout();

private:
  Ui::PixelBatch *ui;

  Settings &m_settings;
  PreferencesWidget *m_preferencesWidget;
  TaskWidget *m_taskWidget;
  TaskActionWidget *m_taskActionWidget;
  QWidget *m_statusBarPermanentWidget;
  QPushButton *m_statusBarAddButton;
  QPushButton *m_statusBarProcessButton;
  QLabel *m_StatusbarPermanentMessageLabel;
  FileHandler *m_fileHandler;

  void initTaskWidget();
  void initTaskActionWidget();
  void setupStatusBar();
  void initMenuBar();
  void initPreferencesWidget();
  void applySavedThemeAndStyle();
};
#endif // PIXELBATCH_H
