#ifndef IMAGEOPTIMIZER_H
#define IMAGEOPTIMIZER_H

#include "imagetype.h"

#include <QString>
#include <QStringList>

class ImageOptimizer {
public:
  ImageOptimizer();
  ImageOptimizer(const QString &name, const QStringList &supportedFormats,
                 const ImageType &imageType);

  QString getName() const;
  QStringList getSupportedFormats() const;
  ImageType getImageType() const;
  bool isValid() const;

private:
  bool valid;
  QString optimizerName;
  QStringList supportedFormats;
  ImageType imageType;
};

#endif // IMAGEOPTIMIZER_H
