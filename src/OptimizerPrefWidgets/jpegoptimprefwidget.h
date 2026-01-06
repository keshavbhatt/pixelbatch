#ifndef JPEGOPTIMPREFWIDGET_H
#define JPEGOPTIMPREFWIDGET_H

#include "imageoptimizerprefwidget.h"
#include "settings.h"

#include <QWidget>

namespace Ui {
class JpegOptimPrefWidget;
}

class JpegOptimPrefWidget : public ImageOptimizerPrefWidget {
  Q_OBJECT

public:
  explicit JpegOptimPrefWidget(QWidget *parent = nullptr);
  ~JpegOptimPrefWidget();

  void loadSettings() override;
  void loadCustomSettings(const QVariantMap &settings) override;
  QVariantMap getCurrentSettings() const override;
  void setAutoSaveEnabled(bool enabled) override;
  void restoreDefaults() override;

private slots:
  void saveSettings();
  void updateQualityLabel(int quality);

private:
  Ui::JpegOptimPrefWidget *ui;
  Settings &m_settings;
  bool m_autoSaveEnabled = true;
};

#endif // JPEGOPTIMPREFWIDGET_H
