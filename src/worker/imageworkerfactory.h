#ifndef IMAGEWORKERFACTORY_H
#define IMAGEWORKERFACTORY_H

#include "ImageWorker.h"
#include "imageoptimizer.h"
#include "imagetype.h"

class ImageWorkerFactory {
public:
  static ImageWorker *getWorker(const QString &filePath);

  static ImageType getImageTypeByExtension(const QString &extension);

  static QList<ImageOptimizer>
  getOptimizersForFormat(const QString &formatName);

  static QList<ImageOptimizer> getRegisteredImageOptimizers();

private:
  static QList<ImageOptimizer> registeredImageOptimizers;
  static QList<ImageOptimizer> createImageOptimizers();
};

#endif // IMAGEWORKERFACTORY_H
