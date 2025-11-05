#ifndef PNGQUANTWORKER_H
#define PNGQUANTWORKER_H

#include "ImageWorker.h"
#include <imagetask.h>

class PngquantWorker : public ImageWorker {
  Q_OBJECT
public:
  explicit PngquantWorker(QObject *parent = nullptr);

  void optimize(ImageTask *task) override;
};

#endif // PNGQUANTWORKER_H
