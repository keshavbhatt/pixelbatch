#include "imageworkerfactory.h"

#include "pngoutworker.h"
#include "pngquantworker.h"

ImageWorker *ImageWorkerFactory::getWorker(const QString &filePath) {
  QFileInfo fileInfo(filePath);
  QString ext = fileInfo.suffix().toLower();

  if (ext == "jpg" || ext == "jpeg") {
      bool allowLossy = true;  //Allow lossy compression
      auto strip = QStringList{"all"}; //strip
      int maxQuality = 100;
    return new JpegoptimWorker(allowLossy, strip, maxQuality);
  }
  else if (ext == "png") {
      // bool allowLossy = true;  //Allow lossy compression
      // int maxColors = 256;     //Maximum number of colors
      // QPair<int, int> qualityRange = qMakePair(0, 100); // Quality range
      // int speed = 3;           // Speed/quality trade-off

      // return new PngquantWorker(allowLossy, maxColors, qualityRange, speed);

      return new PngoutWorker(false, 0);
  }
  // else if (ext == "gif") {
  //   return new GIFProcessor();
  // } else if (ext == "svg") {
  //   return new SVGProcessor();
  // }
  else {
      qWarning() << "Unsupported file type: " + ext;
      throw std::runtime_error("Unsupported file type: " + ext.toStdString());
  }
}
