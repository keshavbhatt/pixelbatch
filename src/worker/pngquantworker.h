#ifndef PNGQUANTWORKER_H
#define PNGQUANTWORKER_H

#include "ImageWorker.h"
#include <QStringList>
#include <imagetask.h>

class PngquantWorker : public ImageWorker {
  Q_OBJECT
public:
  PngquantWorker(bool allowLossy, int maxColors, QPair<int, int> qualityRange,
                 int speed, QObject *parent = nullptr);

  void optimize(ImageTask *task) override;

private:
  bool allowLossy;
  int maxColors;
  QPair<int, int> qualityRange;
  int speed;
};

#endif // PNGQUANTWORKER_H
