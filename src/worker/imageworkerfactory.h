#ifndef IMAGEWORKERFACTORY_H
#define IMAGEWORKERFACTORY_H

#include "ImageWorker.h"
#include "imageoptimizer.h"
#include "imagetype.h"
#include <QFileInfo>
#include <QList>
#include <QMap>
#include <QString>

class ImageWorkerFactory {
public:
  static ImageWorkerFactory &instance();

  ImageWorkerFactory(const ImageWorkerFactory &) = delete;
  ImageWorkerFactory &operator=(const ImageWorkerFactory &) = delete;

  ImageWorker *getWorker(const QString &filePath);
  ImageType getImageTypeByExtension(const QString &extension);
  ImageOptimizer getOptimizerByImageType(ImageType imageType);
  QList<ImageOptimizer> getOptimizersForFormat(const QString &formatName);
  QList<ImageOptimizer> getRegisteredImageOptimizers();

private:
  ImageWorkerFactory();

  static QList<ImageOptimizer> createImageOptimizers();

  QList<ImageOptimizer> registeredImageOptimizers;
};

#endif // IMAGEWORKERFACTORY_H
