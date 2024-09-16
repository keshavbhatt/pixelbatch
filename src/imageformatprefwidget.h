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

  QList<ImageOptimizer> formatOptimizers() const;

private:
  Ui::ImageFormatPrefWidget *ui;
  QString m_formatName;
  QList<ImageOptimizer> m_formatOptimizers;
  void optimizerChanged(int index);
  ImageOptimizer getOptimizerByName(const QString &name);
  void updateFotmatSettingPBTooltip();
};

#endif // IMAGEFORMATPREFWIDGET_H
