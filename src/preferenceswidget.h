#ifndef PREFERENCESWIDGET_H
#define PREFERENCESWIDGET_H

#include "settings.h"

#include <QWidget>

namespace Ui {
class PreferencesWidget;
}

class PreferencesWidget : public QWidget {
  Q_OBJECT

public:
  explicit PreferencesWidget(QWidget *parent = nullptr);
  ~PreferencesWidget();

private slots:
  void onChangeOutputDirPath();
  void onCleanOptimizedFilePrefix();
  void onResetFilepickerLastOpenedPath();

private:
  Ui::PreferencesWidget *ui;

  Settings &m_settings;
};

#endif // PREFERENCESWIDGET_H
