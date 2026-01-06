#ifndef SVGOPREFWIDGET_H
#define SVGOPREFWIDGET_H

#include "imageoptimizerprefwidget.h"
#include "settings.h"

#include <QWidget>

namespace Ui {
class SvgoPrefWidget;
}

class SvgoPrefWidget : public ImageOptimizerPrefWidget {
  Q_OBJECT

public:
  explicit SvgoPrefWidget(QWidget *parent = nullptr);
  ~SvgoPrefWidget();

  void loadSettings() override;
  void loadCustomSettings(const QVariantMap &settings) override;
  QVariantMap getCurrentSettings() const override;
  void setAutoSaveEnabled(bool enabled) override;

private slots:
  void saveSettings();
  void updatePrecisionLabel(int value);

private:
  Ui::SvgoPrefWidget *ui;
  Settings &m_settings;
  bool m_autoSaveEnabled = true;
};

#endif // SVGOPREFWIDGET_H

