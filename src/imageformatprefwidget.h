#ifndef IMAGEFORMATPREFWIDGET_H
#define IMAGEFORMATPREFWIDGET_H

#include <QWidget>

#include <worker/imageoptimizer.h>

namespace Ui {
class ImageFormatPrefWidget;
}

class ImageFormatPrefWidget : public QWidget {
  Q_OBJECT

public:
  explicit ImageFormatPrefWidget(
      QWidget *parent = nullptr, QString formatName = QString(),
      QList<ImageOptimizer> optimizers = QList<ImageOptimizer>());
  ~ImageFormatPrefWidget();

  QString getFormatName() const;
  ImageOptimizer getOptimizer() const;

private:
  Ui::ImageFormatPrefWidget *ui;
  QString m_formatName;
  ImageOptimizer m_optimizer;
  void updateTooltips();
};

#endif // IMAGEFORMATPREFWIDGET_H
