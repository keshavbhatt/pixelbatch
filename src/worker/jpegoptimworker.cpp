#include "jpegoptimworker.h"
#include <QProcess>
#include <imagetask.h>

 // https://github.com/tjko/jpegoptim

JpegoptimWorker::JpegoptimWorker(bool allowLossy, QStringList strip, int maxQuality, QObject *parent)
    : ImageWorker(parent), allowLossy(allowLossy), strip(strip), maxQuality(maxQuality) {}

void JpegoptimWorker::optimize(const ImageTask &task) {
    QString src = task.imagePath;
    QString dst = task.optimizedPath;

    QFile::copy(src, dst);

    QStringList args;
    args << "--quiet" << "--" << dst;

    for (const auto &stripMarker : qAsConst(strip)) {
        args.prepend("--strip-" + stripMarker);
    }

    if (allowLossy && maxQuality < 100) {
        args.prepend("--max=" + QString::number(maxQuality));
    }

    executeProcess("jpegoptim", args, task);
}

