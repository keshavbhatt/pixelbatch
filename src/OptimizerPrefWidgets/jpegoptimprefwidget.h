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

private slots:
  void saveSettings();
  void updateQualityLabel(int quality);

private:
  Ui::JpegOptimPrefWidget *ui;
  Settings &m_settings;
};

#endif // JPEGOPTIMPREFWIDGET_H
