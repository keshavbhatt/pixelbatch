#ifndef IMAGEFORMATPREFWIDGETFACTORY_H
#define IMAGEFORMATPREFWIDGETFACTORY_H

#include <imagetype.h>

#include <worker/imageoptimizer.h>


class ImageFormatPrefWidgetFactory {
public:
  ImageFormatPrefWidgetFactory();

  static void openPrefWidgetFor(const ImageOptimizer &imageOptimizer);
};

#endif // IMAGEFORMATPREFWIDGETFACTORY_H
