#include "imageworkerfactory.h"

#include "jpegoptimworker.h"
#include "pngoutworker.h"
#include "pngquantworker.h"

QMap<QString, ImageWorkerFactory::ImageType> ImageWorkerFactory::extensionMap =
    ImageWorkerFactory::createExtensionMap();
QList<QString> ImageWorkerFactory::extensionList =
    ImageWorkerFactory::createExtensionList();
QMap<QString, QStringList> ImageWorkerFactory::optimizersMap =
    ImageWorkerFactory::createOptimizersMap();

QMap<QString, ImageWorkerFactory::ImageType>
ImageWorkerFactory::createExtensionMap() {
  QMap<QString, ImageType> map;
  map["jpg"] = ImageType::JPG;
  map["jpeg"] = ImageType::JPG;
  map["png"] = ImageType::PNG;
  map["gif"] = ImageType::GIF;
  map["svg"] = ImageType::SVG;
  return map;
}

QList<QString> ImageWorkerFactory::createExtensionList() {
  return QList<QString>{"jpg/jpeg", "png", "gif", "svg"};
}

ImageWorkerFactory::ImageType
ImageWorkerFactory::getImageType(const QString &extension) {
  auto it = extensionMap.find(extension.toLower());
  if (it != extensionMap.end()) {
    return it.value();
  } else {
    return ImageType::Unsupported;
  }
}

QMap<QString, QStringList> ImageWorkerFactory::createOptimizersMap() {
  QMap<QString, QStringList> map;
  map["jpg/jpeg"] = QStringList{"Jpegoptim"};
  map["png"] = QStringList{"Pngquant", "Pngout"};
  map["gif"] = QStringList{"GIFProcessor"};
  map["svg"] = QStringList{"SVGProcessor"};
  return map;
}

QStringList
ImageWorkerFactory::getOptimizersForFormat(const QString &formatName) {
  auto it = optimizersMap.find(formatName.toLower());
  if (it != optimizersMap.end()) {
    return it.value();
  } else {
    qWarning() << "Unsupported format: " + formatName;
    throw std::runtime_error("Unsupported format: " + formatName.toStdString());
  }
}

QStringList ImageWorkerFactory::getSupportedFormats() {
  return QStringList(extensionList);
}

ImageWorker *ImageWorkerFactory::getWorker(const QString &filePath) {
  QFileInfo fileInfo(filePath);
  QString ext = fileInfo.suffix().toLower();
  ImageType type = getImageType(ext);

  switch (type) {
  case ImageType::JPG: {
    bool allowLossy = true;
    auto strip = QStringList{"all"};
    int maxQuality = 100;
    return new JpegoptimWorker(allowLossy, strip, maxQuality);
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
