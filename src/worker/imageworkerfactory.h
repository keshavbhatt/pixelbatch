#ifndef IMAGEWORKERFACTORY_H
#define IMAGEWORKERFACTORY_H

#include "ImageWorker.h"
#include "jpegoptimworker.h"

class ImageWorkerFactory {
public:
  static ImageWorker *getWorker(const QString &filePath);
};

#endif // IMAGEWORKERFACTORY_H
