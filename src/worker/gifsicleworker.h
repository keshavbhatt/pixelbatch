#ifndef GIFSICLEWORKER_H
#define GIFSICLEWORKER_H

#include "ImageWorker.h"
#include <imagetask.h>

class GifsicleWorker : public ImageWorker {
  Q_OBJECT
public:
  explicit GifsicleWorker(QObject *parent = nullptr);

  void optimize(ImageTask *task) override;
};

#endif // GIFSICLEWORKER_H

