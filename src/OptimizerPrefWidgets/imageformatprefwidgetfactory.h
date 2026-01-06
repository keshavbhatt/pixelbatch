#ifndef IMAGEFORMATPREFWIDGETFACTORY_H
#define IMAGEFORMATPREFWIDGETFACTORY_H

#include "imageoptimizerprefwidget.h"

#include <QMap>
#include <QString>
#include <functional>
#include <imagetype.h>
#include <worker/imageoptimizer.h>

class ImageFormatPrefWidgetFactory {
public:
  static ImageFormatPrefWidgetFactory &instance();

  void openPrefWidgetFor(const ImageOptimizer &imageOptimizer);

  QWidget *createPrefWidgetFor(const ImageOptimizer &imageOptimizer,
                                QWidget *parent = nullptr,
                                bool wrapInScrollArea = true);

  ImageOptimizerPrefWidget *createOptimizerWidget(const ImageOptimizer &imageOptimizer,
                                                   QWidget *parent = nullptr);

  bool hasPrefWidgetFor(const ImageOptimizer &imageOptimizer) const;

private:
  ImageFormatPrefWidgetFactory();

  ImageFormatPrefWidgetFactory(const ImageFormatPrefWidgetFactory &) = delete;
  ImageFormatPrefWidgetFactory &
  operator=(const ImageFormatPrefWidgetFactory &) = delete;

  QMap<QString, std::function<ImageOptimizerPrefWidget *(QWidget *)>>
      optimizerWidgetMap;

  void registerOptimizers();

  using WidgetCreator = std::function<ImageOptimizerPrefWidget *(QWidget *)>;
};

#endif // IMAGEFORMATPREFWIDGETFACTORY_H
