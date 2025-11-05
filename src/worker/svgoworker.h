#ifndef SVGOWORKER_H
#define SVGOWORKER_H

#include "ImageWorker.h"
#include <imagetask.h>

class SvgoWorker : public ImageWorker {
  Q_OBJECT
public:
  explicit SvgoWorker(QObject *parent = nullptr);

  void optimize(ImageTask *task) override;
};

#endif // SVGOWORKER_H

