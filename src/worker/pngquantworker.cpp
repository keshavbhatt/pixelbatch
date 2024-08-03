#include "pngquantworker.h"
#include <QProcess>

// https://pngquant.org/

PngquantWorker::PngquantWorker(bool allowLossy, int maxColors, QPair<int, int> qualityRange, int speed, QObject *parent)
    : ImageWorker(parent), allowLossy(allowLossy), maxColors(maxColors), qualityRange(qualityRange), speed(speed) {}

void PngquantWorker::optimize(ImageTask *task) {
    QString src = task->imagePath;
    QString dst = task->optimizedPath;

    QStringList args;
    args << "--quality=" + QString::number(qualityRange.first) + "-" + QString::number(qualityRange.second)
         << "--speed=" + QString::number(speed)
         << "--output=" + dst
         << "--force"  // Overwrite existing output files
         << "--strip"  // Remove optional metadata
         << "--"
         << src;

    //<< "--skip-if-larger"  // Only save if smaller

    if (!allowLossy) {
        args.prepend("--nofs"); // Disable Floyd-Steinberg dithering if not allowing lossy
    }

    executeProcess("pngquant", args, task);
}

