#include "imageoptimizer.h"

ImageOptimizer::ImageOptimizer() { valid = false; }

ImageOptimizer::ImageOptimizer(const QString &name,
                               const QStringList &supportedFormats,
                               const ImageType &imageType)
    : optimizerName(name), supportedFormats(supportedFormats),
      imageType(imageType) {
  valid = true;
}

QString ImageOptimizer::getName() const { return optimizerName; }

QStringList ImageOptimizer::getSupportedFormats() const {
  return supportedFormats;
}

ImageType ImageOptimizer::getImageType() const { return imageType; }

bool ImageOptimizer::isValid() const { return valid; }
