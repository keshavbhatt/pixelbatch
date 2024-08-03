#ifndef PNGOUTWORKER_H
#define PNGOUTWORKER_H

#include "ImageWorker.h"

class PngoutWorker : public ImageWorker {
    Q_OBJECT
public:
    PngoutWorker(bool copyChunks, int strategy, QObject *parent = nullptr);

    void optimize(ImageTask *task) override;

private:
    bool copyChunks;
    int strategy;
};

#endif // PNGOUTWORKER_H
