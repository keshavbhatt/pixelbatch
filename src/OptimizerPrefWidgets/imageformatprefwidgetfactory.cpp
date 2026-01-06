#include "imageformatprefwidgetfactory.h"
#include "gifsicleprefwidget.h"
#include "jpegoptimprefwidget.h"
#include "pngquantprefwidget.h"
#include "svgoprefwidget.h"
#include "worker/imageworkerfactory.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

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
      // Wrap the widget in a QScrollArea to handle overflow on small screens
      QScrollArea *scrollArea = new QScrollArea();
      scrollArea->setWidget(widget);
      scrollArea->setWidgetResizable(true);
      scrollArea->setWindowTitle(optimizerName + " " + QObject::tr("Preferences"));

      widget->loadSettings();
      scrollArea->show();
      qDebug() << "Opened preference widget for" << optimizerName;
    }
  } else {
    qDebug() << "No preference widget registered for optimizer:"

             << optimizerName;
  }
}

QWidget *ImageFormatPrefWidgetFactory::createPrefWidgetFor(
    const ImageOptimizer &imageOptimizer, QWidget *parent, bool wrapInScrollArea) {
  QString optimizerName = imageOptimizer.getName();

  if (optimizerWidgetMap.contains(optimizerName)) {
    ImageOptimizerPrefWidget *widget =
        optimizerWidgetMap[optimizerName](parent);
    if (widget) {
      widget->loadSettings();
      qDebug() << "Created preference widget for" << optimizerName;

      if (wrapInScrollArea) {
        // Wrap the widget in a QScrollArea to handle overflow on small screens
        QScrollArea *scrollArea = new QScrollArea(parent);
        scrollArea->setWidget(widget);
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameShape(QFrame::NoFrame);
        return scrollArea;
      } else {
        // Return widget directly without scroll area wrapping
        return widget;
      }
    }
  } else {
    qDebug() << "No preference widget registered for optimizer:"
             << optimizerName;
  }

  return nullptr;
}

ImageOptimizerPrefWidget *ImageFormatPrefWidgetFactory::createOptimizerWidget(
    const ImageOptimizer &imageOptimizer, QWidget *parent) {
  QString optimizerName = imageOptimizer.getName();

  if (optimizerWidgetMap.contains(optimizerName)) {
    ImageOptimizerPrefWidget *widget =
        optimizerWidgetMap[optimizerName](parent);
    if (widget) {
      qDebug() << "Created optimizer widget for" << optimizerName;
      return widget;
    }
  } else {
    qDebug() << "No optimizer widget registered for:" << optimizerName;
  }

  return nullptr;
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
    } else if (optimizerName == "Gifsicle") {
      optimizerWidgetMap[optimizerName] =
          [](QWidget *parent) -> ImageOptimizerPrefWidget * {
        return new GifsiclePrefWidget(parent);
      };
    } else if (optimizerName == "SVGO") {
      optimizerWidgetMap[optimizerName] =
          [](QWidget *parent) -> ImageOptimizerPrefWidget * {
        return new SvgoPrefWidget(parent);
      };
    }
  }
}

bool ImageFormatPrefWidgetFactory::hasPrefWidgetFor(
    const ImageOptimizer &imageOptimizer) const {
  QString optimizerName = imageOptimizer.getName();
  return optimizerWidgetMap.contains(optimizerName);
}
