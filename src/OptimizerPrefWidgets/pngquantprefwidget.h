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

private slots:
  void saveSettings();
  void updateQualityRangeLabel();

private:
  Ui::PngQuantPrefWidget *ui;
  Settings &m_settings;
};

#endif // PNGQUANTPREFWIDGET_H

