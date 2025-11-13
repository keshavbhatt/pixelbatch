#include "imageworkerfactory.h"
#include "jpegoptimworker.h"
#include "pngquantworker.h"
#include "gifsicleworker.h"
#include "svgoworker.h"
#include <QDebug>
#include <stdexcept>

ImageWorkerFactory &ImageWorkerFactory::instance() {
  static ImageWorkerFactory factoryInstance;
  return factoryInstance;
}

ImageWorkerFactory::ImageWorkerFactory() {
  registeredImageOptimizers = createImageOptimizers();
}

QList<ImageOptimizer> ImageWorkerFactory::createImageOptimizers() {
  QList<ImageOptimizer> optimizers;
  optimizers.append(
      ImageOptimizer("Jpegoptim", QStringList{"jpg", "jpeg"}, ImageType::JPG));
  optimizers.append(
      ImageOptimizer("Pngquant", QStringList{"png"}, ImageType::PNG));
  optimizers.append(
      ImageOptimizer("Gifsicle", QStringList{"gif"}, ImageType::GIF));
  optimizers.append(
      ImageOptimizer("SVGO", QStringList{"svg"}, ImageType::SVG));

  return optimizers;
}

ImageType
ImageWorkerFactory::getImageTypeByExtension(const QString &extension) {
  foreach (auto optimizer, registeredImageOptimizers) {
    foreach (auto ext, optimizer.getSupportedFormats()) {
      if (ext.toLower() == extension.toLower()) {
        return optimizer.getImageType();
      }
    }
  }
  return ImageType::Unsupported;
}

ImageOptimizer ImageWorkerFactory::getOptimizerByImageType(ImageType imageType) {
  foreach (auto optimizer, registeredImageOptimizers) {
    if (optimizer.getImageType() == imageType) {
      return optimizer;
    }
  }
  // Return a default/invalid optimizer if not found
  return ImageOptimizer("", QStringList{}, ImageType::Unsupported);
}

QList<ImageOptimizer> ImageWorkerFactory::getRegisteredImageOptimizers() {
  return registeredImageOptimizers;
}

QList<ImageOptimizer>
ImageWorkerFactory::getOptimizersForFormat(const QString &formatName) {
  QList<ImageOptimizer> matches;
  foreach (auto optimizer, registeredImageOptimizers) {
    if (optimizer.getSupportedFormats().contains(formatName,
                                                 Qt::CaseInsensitive)) {
      matches.append(optimizer);
    }
  }

  if (!matches.isEmpty()) {
    return matches;
  } else {
    qWarning() << "Unsupported format: " + formatName;
    throw std::runtime_error("Unsupported format: " + formatName.toStdString());
  }
}

ImageWorker *ImageWorkerFactory::getWorker(const QString &filePath) {
  QFileInfo fileInfo(filePath);
  QString ext = fileInfo.suffix().toLower();
  ImageType type = getImageTypeByExtension(ext);

  switch (type) {
  case ImageType::JPG: {
    return new JpegoptimWorker();
  }
  case ImageType::PNG: {
    return new PngquantWorker();
  }
  case ImageType::GIF: {
    return new GifsicleWorker();
  }
  case ImageType::SVG: {
    return new SvgoWorker();
  }
  case ImageType::Unsupported:
  default:
    qWarning() << "Unsupported file type: " + ext;
    throw std::runtime_error("Unsupported file type: " + ext.toStdString());
  }
}
