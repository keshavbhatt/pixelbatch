#include "imageworkerfactory.h"
#include "jpegoptimworker.h"
#include "pngquantworker.h"
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
      ImageOptimizer("Pngout", QStringList{"png"}, ImageType::PNG));
  optimizers.append(
      ImageOptimizer("GIFProcessor", QStringList{"gif"}, ImageType::GIF));
  optimizers.append(
      ImageOptimizer("SVGProcessor", QStringList{"svg"}, ImageType::SVG));

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
    bool allowLossy = true;
    int maxColors = 256;
    QPair<int, int> qualityRange = qMakePair(0, 100);
    int speed = 3;
    return new PngquantWorker(allowLossy, maxColors, qualityRange, speed);
  }
  // case ImageType::GIF:
  //   return new GIFProcessor();
  // case ImageType::SVG:
  //   return new SVGProcessor();
  case ImageType::Unsupported:
  default:
    qWarning() << "Unsupported file type: " + ext;
    throw std::runtime_error("Unsupported file type: " + ext.toStdString());
  }
}
