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

private:
  Ui::JpegOptimPrefWidget *ui;

  Settings &m_settings;

  // ImageOptimizerPrefWidget interface
public:
  void loadSettings();
};

#endif // JPEGOPTIMPREFWIDGET_H
