#ifndef JPEGOPTIMPREFWIDGET_H
#define JPEGOPTIMPREFWIDGET_H

#include "imageoptimizerprefwidget.h"

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

    // ImageOptimizerPrefWidget interface
public:
    void loadSettings();
};

#endif // JPEGOPTIMPREFWIDGET_H
