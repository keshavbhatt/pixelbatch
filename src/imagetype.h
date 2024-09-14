#ifndef IMAGETYPE_H
#define IMAGETYPE_H

#include <QList>

enum class ImageType { JPG, PNG, GIF, SVG, Unsupported };

class ImageTypeUtils {
public:
  static QList<ImageType> getSupportedImageTypes() {
    return QList<ImageType>{ImageType::JPG, ImageType::PNG, ImageType::GIF,
                            ImageType::SVG};
  }

  static QString imageTypeToString(ImageType type) {
    switch (type) {
    case ImageType::JPG:
      return "JPG";
    case ImageType::PNG:
      return "PNG";
    case ImageType::GIF:
      return "GIF";
    case ImageType::SVG:
      return "SVG";
    default:
      return "Unknown";
    }
  }
};

#endif // IMAGETYPE_H
