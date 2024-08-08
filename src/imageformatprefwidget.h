#ifndef IMAGEFORMATPREFWIDGET_H
#define IMAGEFORMATPREFWIDGET_H

#include <QWidget>

namespace Ui {
class ImageFormatPrefWidget;
}

class ImageFormatPrefWidget : public QWidget {
  Q_OBJECT

public:
  explicit ImageFormatPrefWidget(QWidget *parent = nullptr,
                                 QString formatName = QString(),
                                 QStringList optimizers = QStringList());
  ~ImageFormatPrefWidget();

  QString getFormatName() const;

  QStringList formatOptimizers() const;

private:
  Ui::ImageFormatPrefWidget *ui;
  QString m_formatName;
  QStringList m_formatOptimizers;
  void optimizerChanged(int);
};

#endif // IMAGEFORMATPREFWIDGET_H
