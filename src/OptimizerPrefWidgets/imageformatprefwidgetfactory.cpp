#include "imageformatprefwidgetfactory.h"
#include "jpegoptimprefwidget.h"
#include "pngquantprefwidget.h"
#include "worker/imageworkerfactory.h"
#include <QDebug>

ImageFormatPrefWidgetFactory &ImageFormatPrefWidgetFactory::instance() {
  static ImageFormatPrefWidgetFactory instance;
  return instance;
}

ImageFormatPrefWidgetFactory::ImageFormatPrefWidgetFactory() {
  registerOptimizers();
}

void ImageFormatPrefWidgetFactory::openPrefWidgetFor(
    const ImageOptimizer &imageOptimizer) {
  QString optimizerName = imageOptimizer.getName();

  if (optimizerWidgetMap.contains(optimizerName)) {
    ImageOptimizerPrefWidget *widget =
        optimizerWidgetMap[optimizerName](nullptr);
    if (widget) {
      widget->setWindowTitle(optimizerName + " " + QObject::tr("Preferences"));
      widget->loadSettings();
      widget->show();
      qDebug() << "Opened preference widget for" << optimizerName;
    }
  } else {
    qDebug() << "No preference widget registered for optimizer:"
             << optimizerName;
  }
}

void ImageFormatPrefWidgetFactory::registerOptimizers() {
  QList<ImageOptimizer> registeredImageOptimizers =
      ImageWorkerFactory::instance().getRegisteredImageOptimizers();

  for (const ImageOptimizer &optimizer : qAsConst(registeredImageOptimizers)) {
    QString optimizerName = optimizer.getName();

    if (optimizerName == "Jpegoptim") {
      optimizerWidgetMap[optimizerName] =
          [](QWidget *parent) -> ImageOptimizerPrefWidget * {
        return new JpegOptimPrefWidget(parent);
      };
    } else if (optimizerName == "Pngquant") {
      optimizerWidgetMap[optimizerName] =
          [](QWidget *parent) -> ImageOptimizerPrefWidget * {
        return new PngQuantPrefWidget(parent);
      };
    }
    //} else if (optimizerName == "GIFProcessor") {
    //   optimizerWidgetMap[optimizerName] =
    //       [](QWidget *parent) -> ImageOptimizerPrefWidget * {
    //     return new GIFProcessorPrefWidget(parent);
    //   };
    // } else if (optimizerName == "SVGProcessor") {
    //   optimizerWidgetMap[optimizerName] =
    //       [](QWidget *parent) -> ImageOptimizerPrefWidget * {
    //     return new SVGProcessorPrefWidget(parent);
    //   };
    // }
  }
}

bool ImageFormatPrefWidgetFactory::hasPrefWidgetFor(
    const ImageOptimizer &imageOptimizer) const {
  QString optimizerName = imageOptimizer.getName();
  return optimizerWidgetMap.contains(optimizerName);
}
