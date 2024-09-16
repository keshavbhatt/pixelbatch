#ifndef IMAGEOPTIMIZERPREFWIDGET_H
#define IMAGEOPTIMIZERPREFWIDGET_H

#include <QWidget>

class ImageOptimizerPrefWidget : public QWidget {
  Q_OBJECT

public:
  explicit ImageOptimizerPrefWidget(QWidget *parent = nullptr);
  virtual ~ImageOptimizerPrefWidget() {}
  virtual void loadSettings() = 0;
};

#endif // IMAGEOPTIMIZERPREFWIDGET_H
