#ifndef JPEGOPTIMWORKER_H
#define JPEGOPTIMWORKER_H

#include "ImageWorker.h"
#include "settings.h"

class JpegoptimWorker : public ImageWorker {
  Q_OBJECT
public:
  JpegoptimWorker(QObject *parent = nullptr);

  void optimize(ImageTask *task) override;
};

#endif // JPEGOPTIMWORKER_H
