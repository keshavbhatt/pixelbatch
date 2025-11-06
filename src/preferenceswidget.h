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
  void onThemeChanged(int index);
  void onStyleChanged(int index);
  void onUseSystemThemeToggled(bool checked);

private:
  Ui::PreferencesWidget *ui;

  Settings &m_settings;

  void applyTheme(const QString &theme, bool useSystemTheme = false);
  void applyStyle(const QString &style, bool useSystemTheme = false);
};

#endif // PREFERENCESWIDGET_H
