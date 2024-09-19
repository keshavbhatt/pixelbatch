#include "jpegoptimworker.h"
#include <QProcess>
#include <imagetask.h>

/**
 * https://github.com/tjko/jpegoptim
 * based on jpegoptim v1.5.5
 *
 * @brief JpegoptimWorker::JpegoptimWorker
 * @param parent
 */
JpegoptimWorker::JpegoptimWorker(QObject *parent) : ImageWorker(parent) {}

void JpegoptimWorker::optimize(ImageTask *task) {
  QString src = task->imagePath;
  QString dst = task->optimizedPath;

  // By default jpegoptim modifies the input files, so we operate on copied file
  // in destination. If process fails we remove the copied file
  QFile::copy(src, dst);

  const QSettings &settings = Settings::instance().getSettings();

  bool forceOptimization = settings.value("jpegoptim/force", false).toBool();
  int maxQuality = settings.value("jpegoptim/maxQuality", 100).toInt();
  int targetSize = settings.value("jpegoptim/targetSize", 0).toInt();
  int compressionThreshold =
      settings.value("jpegoptim/compressionThreshold", 0).toInt();
  int maxWorkers = settings.value("jpegoptim/maxWorkers", 1).toInt();
  bool preserveTimes =
      settings.value("jpegoptim/preserveTimes", false).toBool();
  // bool allNormal = settings.value("jpegoptim/allNormal", false).toBool();
  // bool allProgressive =
  //     settings.value("jpegoptim/allProgressive", false).toBool();
  // bool allArith = settings.value("jpegoptim/allArith", false).toBool();
  // bool allHuffman = settings.value("jpegoptim/allHuffman", false).toBool();
  // bool noFix = settings.value("jpegoptim/noFix", false).toBool();
  // bool preservePerms = settings.value("jpegoptim/preservePerms",
  // true).toBool();

  QStringList args;

  // Basic quiet mode
  args << "--quiet" << "--" << dst;

  // Force optimization
  if (forceOptimization) {
    args.prepend("--force");
  }

  // Max quality
  if (maxQuality < 100) {
    args.prepend("--max=" + QString::number(maxQuality));
  }

  // Target size
  if (targetSize > 0) {
    args.prepend("--size=" + QString::number(targetSize));
  }

  // Compression threshold
  if (compressionThreshold > 0) {
    args.prepend("--threshold=" + QString::number(compressionThreshold));
  }

  // Max workers
  if (maxWorkers > 1) {
    args.prepend("--workers=" + QString::number(maxWorkers));
  }

  // Preserve file times
  if (preserveTimes) {
    args.prepend("--preserve");
  }

  // // Force all output files to be non-progressive
  // if (allNormal) {
  //   args.prepend("--all-normal");
  // }

  // // Force all output files to be progressive
  // if (allProgressive) {
  //   args.prepend("--all-progressive");
  // }

  // // Force all output files to use Arithmetic Coding
  // if (allArith) {
  //   args.prepend("--all-arith");
  // }

  // // Force all output files to use (traditional) Huffman coding
  // if (allHuffman) {
  //   args.prepend("--all-huffman");
  // }

  // Skip processing of any input files that have any errors/warnings during
  // decompression.
  // if (noFix) {
  //   args.prepend("--nofix");
  // }

  // // Preserve file permissions
  // if (preservePerms) {
  //   args.prepend("--preserve-perms");
  // }

  executeProcess("jpegoptim", args, task);
}
