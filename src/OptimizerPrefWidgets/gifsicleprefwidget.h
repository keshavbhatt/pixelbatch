#ifndef GIFSICLEPREFWIDGET_H
#define GIFSICLEPREFWIDGET_H

#include "imageoptimizerprefwidget.h"
#include "settings.h"

#include <QWidget>

namespace Ui {
class GifsiclePrefWidget;
}

class GifsiclePrefWidget : public ImageOptimizerPrefWidget {
  Q_OBJECT

public:
  explicit GifsiclePrefWidget(QWidget *parent = nullptr);
  ~GifsiclePrefWidget();

  void loadSettings() override;

private slots:
  void saveSettings();
  void updateLossyControls(int compressionType);
  void updateColorReductionControls(bool enabled);
  void updateLossyLevelLabel(int value);

private:
  Ui::GifsiclePrefWidget *ui;
  Settings &m_settings;
};

#endif // GIFSICLEPREFWIDGET_H

