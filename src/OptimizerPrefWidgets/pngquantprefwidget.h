#ifndef PNGQUANTPREFWIDGET_H
#define PNGQUANTPREFWIDGET_H

#include "imageoptimizerprefwidget.h"
#include "settings.h"

#include <QWidget>

namespace Ui {
class PngQuantPrefWidget;
}

class PngQuantPrefWidget : public ImageOptimizerPrefWidget {
  Q_OBJECT

public:
  explicit PngQuantPrefWidget(QWidget *parent = nullptr);
  ~PngQuantPrefWidget();

  void loadSettings() override;
  void loadCustomSettings(const QVariantMap &settings) override;
  QVariantMap getCurrentSettings() const override;
  void setAutoSaveEnabled(bool enabled) override;
  void restoreDefaults() override;

private slots:
  void saveSettings();
  void updateQualityRangeLabel();

private:
  Ui::PngQuantPrefWidget *ui;
  Settings &m_settings;
  bool m_autoSaveEnabled = true;
};

#endif // PNGQUANTPREFWIDGET_H

