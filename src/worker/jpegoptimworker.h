#ifndef JPEGOPTIMWORKER_H
#define JPEGOPTIMWORKER_H

#include "ImageWorker.h"
#include <QStringList>

class JpegoptimWorker : public ImageWorker {
    Q_OBJECT
public:
    JpegoptimWorker(bool allowLossy, QStringList strip, int maxQuality, QObject *parent = nullptr);

    void optimize(ImageTask *task) override;

private:
    bool allowLossy;
    QStringList strip;
    int maxQuality;
};

#endif // JPEGOPTIMWORKER_H
