#ifndef IMAGEWORKERFACTORY_H
#define IMAGEWORKERFACTORY_H

#include "ImageWorker.h"

class ImageWorkerFactory {
public:
  enum class ImageType { JPG, PNG, GIF, SVG, Unsupported };

  static ImageWorker *getWorker(const QString &filePath);

  static QStringList getSupportedFormats();

  static ImageType getImageType(const QString &extension);

  static QStringList getOptimizersForFormat(const QString &formatName);

private:
  static QMap<QString, ImageType> createExtensionMap();
  static QMap<QString, ImageType> extensionMap;
  static QList<QString> createExtensionList();
  static QList<QString> extensionList;
  static QMap<QString, QStringList> optimizersMap;
  static QMap<QString, QStringList> createOptimizersMap();
};

#endif // IMAGEWORKERFACTORY_H
