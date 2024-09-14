#include "imageformatprefwidgetfactory.h"

#include <QDebug>

ImageFormatPrefWidgetFactory::ImageFormatPrefWidgetFactory() {}

void ImageFormatPrefWidgetFactory::openPrefWidgetFor(
    const ImageOptimizer &imageOptimizer) {
  qDebug() << "Open pref widget for" << imageOptimizer.getName();
  Q_UNIMPLEMENTED();
}
