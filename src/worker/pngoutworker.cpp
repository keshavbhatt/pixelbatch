#include "pngoutworker.h"

#include <QProcess>
#include <QDebug>

// http://www.advsys.net/ken/util/pngout.htm

PngoutWorker::PngoutWorker(bool copyChunks, int strategy, QObject *parent)
    : ImageWorker(parent), copyChunks(copyChunks), strategy(strategy) {}

void PngoutWorker::optimize(const ImageTask &task) {
    QString src = task.imagePath;
    QString dst = task.optimizedPath;

    QStringList args;
    args << "-k" + QString::number(copyChunks ? 1 : 0)
         << "-s" + QString::number(strategy)
         << "-q"
         << "-y"
         << src
         << dst;

    executeProcess("pngout", args, task);
}

