#include "jpegoptimworker.h"
#include <QProcess>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <imagetask.h>

/**
 * https://github.com/tjko/jpegoptim
 * Based on jpegoptim v1.5.6
 *
 * JPEG Optimization Worker
 *
 * This worker uses jpegoptim to optimize JPEG images. It supports both lossless
 * and lossy compression with extensive configuration options.
 *
 * Key Features:
 * - Lossless optimization (default with maxQuality=100)
 * - Lossy optimization with quality control
 * - Metadata stripping (EXIF, IPTC, ICC, etc.)
 * - Progressive/baseline JPEG output
 * - Target file size optimization
 * - Parallel processing within jpegoptim
 *
 * Settings Mapping:
 *
 * Quality Settings:
 *   - jpegoptim/maxQuality (0-100, default: 100)
 *     → --max=N (if < 100, enables lossy mode)
 *   - jpegoptim/targetSize (KB, default: 0)
 *     → --size=N (if > 0, disables lossless mode)
 *   - jpegoptim/compressionThreshold (%, default: 0)
 *     → --threshold=N (keep original if savings below N%)
 *
 * Metadata Handling (jpegoptim/metadataMode, default: 0):
 *   Mode 0: Keep all    → --strip-none
 *   Mode 1: Strip all   → --strip-all
 *   Mode 2: EXIF only   → --strip-all --keep-exif
 *   Mode 3: ICC only    → --strip-all --keep-icc
 *   Mode 4: EXIF + ICC  → --strip-all --keep-exif --keep-icc
 *
 * Output Format (jpegoptim/outputMode, default: 0):
 *   Mode 0: Auto        → --auto-mode
 *   Mode 1: Progressive → --all-progressive
 *   Mode 2: Baseline    → --all-normal
 *
 * Additional Options:
 *   - jpegoptim/force (default: false)
 *     → --force (force optimization even if already optimized)
 *   - jpegoptim/preserveTimes (default: true)
 *     → --preserve (keep file modification timestamps)
 *   - jpegoptim/retry (default: false)
 *     → --retry (recursively optimize until no improvement)
 *
 * Performance:
 *   - jpegoptim/maxWorkers (1-16, default: 1)
 *     → --workers=N (parallel threads within jpegoptim)
 *
 * Note: jpegoptim operates in-place, so we copy the source to destination
 * first and optimize the copy.
 *
 * @brief JpegoptimWorker::JpegoptimWorker
 * @param parent
 */
JpegoptimWorker::JpegoptimWorker(QObject *parent) : ImageWorker(parent) {}

void JpegoptimWorker::optimize(ImageTask *task) {
  QString src = task->imagePath;
  QString dst = task->optimizedPath;

  // Ensure destination directory exists
  QFileInfo dstInfo(dst);
  QDir dstDir = dstInfo.absoluteDir();
  if (!dstDir.exists()) {
    dstDir.mkpath(".");
  }

  // Copy source to destination first
  // jpegoptim operates in-place, so we work on the destination copy
  if (QFile::exists(dst)) {
    QFile::remove(dst);
  }

  if (!QFile::copy(src, dst)) {
    emit optimizationError(task, "Failed to copy file to destination");
    return;
  }

  // Load settings (using getSetting which checks custom settings first, then falls back to global)
  // Quality settings
  int maxQuality = getSetting("jpegoptim/maxQuality", 100).toInt();
  int targetSize = getSetting("jpegoptim/targetSize", 0).toInt();
  int compressionThreshold = getSetting("jpegoptim/compressionThreshold", 0).toInt();

  // Metadata handling
  int metadataMode = getSetting("jpegoptim/metadataMode", 0).toInt();

  // Output format
  int outputMode = getSetting("jpegoptim/outputMode", 0).toInt();

  // Additional options
  bool forceOptimization = getSetting("jpegoptim/force", false).toBool();
  bool preserveTimes = getSetting("jpegoptim/preserveTimes", true).toBool();
  bool retry = getSetting("jpegoptim/retry", false).toBool();

  // Performance
  int maxWorkers = getSetting("jpegoptim/maxWorkers", 1).toInt();

  // Build arguments
  QStringList args;

  // Always quiet mode for cleaner output
  args << "--quiet";

  // Force optimization if requested
  if (forceOptimization) {
    args << "--force";
  }

  // Max quality (if < 100, enables lossy mode)
  if (maxQuality < 100) {
    args << "--max=" + QString::number(maxQuality);
  }

  // Target size (in KB, disables lossless mode)
  if (targetSize > 0) {
    args << "--size=" + QString::number(targetSize);
  }

  // Compression threshold (keep old file if gain is below threshold %)
  if (compressionThreshold > 0) {
    args << "--threshold=" + QString::number(compressionThreshold);
  }

  // Metadata handling
  // Mode 0: Keep all (default, no args needed)
  // Mode 1: Strip all
  // Mode 2: Keep EXIF only
  // Mode 3: Keep ICC only
  // Mode 4: Keep EXIF + ICC
  switch (metadataMode) {
    case 1: // Strip all
      args << "--strip-all";
      break;
    case 2: // Keep EXIF only
      args << "--strip-all" << "--keep-exif";
      break;
    case 3: // Keep ICC only
      args << "--strip-all" << "--keep-icc";
      break;
    case 4: // Keep EXIF + ICC
      args << "--strip-all" << "--keep-exif" << "--keep-icc";
      break;
    case 0: // Keep all
    default:
      args << "--strip-none";
      break;
  }

  // Output format
  // Mode 0: Auto (default)
  // Mode 1: Progressive
  // Mode 2: Normal/Baseline
  switch (outputMode) {
    case 1: // Progressive
      args << "--all-progressive";
      break;
    case 2: // Normal/Baseline
      args << "--all-normal";
      break;
    case 0: // Auto
    default:
      args << "--auto-mode";
      break;
  }

  // Preserve file timestamps
  if (preserveTimes) {
    args << "--preserve";
  }

  // Retry optimization until no improvement
  if (retry) {
    args << "--retry";
  }

  // Max workers (parallel threads within jpegoptim)
  if (maxWorkers > 1) {
    args << "--workers=" + QString::number(maxWorkers);
  }

  // Add the file to optimize (last argument)
  args << "--" << dst;

  // Debug output
  qDebug() << "jpegoptim command:" << "jpegoptim" << args.join(" ");

  // Execute the optimization
  executeProcess("jpegoptim", args, task);
}
