#include "pngquantworker.h"
#include "settings.h"
#include <QProcess>
#include <QFileInfo>
#include <QFile>
#include <QDir>

/**
 * https://pngquant.org/
 * Based on pngquant 3.0.3 (Rust)
 *
 * PNG Optimization Worker using pngquant
 *
 * pngquant is a lossy PNG compressor that significantly reduces file sizes
 * while maintaining visual quality through advanced quantization and dithering.
 *
 * Key Features:
 * - Lossy compression with quality control (can achieve 40-70% reduction)
 * - Floyd-Steinberg dithering for smooth gradients
 * - Speed/quality trade-off configuration
 * - Smart skip-if-larger to avoid increasing file size
 * - Metadata stripping for smaller files
 *
 * Settings Mapping:
 *
 * Quality Range (pngquant/qualityMin, pngquant/qualityMax):
 *   - Min: 0-100 (default: 65) - Don't save below this quality
 *   - Max: 0-100 (default: 80) - Use fewer colors below this quality
 *   → --quality=MIN-MAX
 *
 * Speed vs Quality (pngquant/speed, default: 4):
 *   - 1 = Slowest, best quality
 *   - 4 = Default, balanced
 *   - 11 = Fastest, rougher quality
 *   → --speed=N
 *
 * Dithering (pngquant/enableDithering, default: true):
 *   - true = Enable Floyd-Steinberg dithering (better gradients)
 *   - false = Disable dithering (pixel art, simple graphics)
 *   → --nofs (when disabled)
 *
 * Posterization (pngquant/posterize, default: 0):
 *   - 0 = Disabled
 *   - 1-8 = Reduced color precision (for ARGB4444 output, etc.)
 *   → --posterize=N (when > 0)
 *
 * Additional Options:
 *   - pngquant/stripMetadata (default: true) → --strip
 *   - pngquant/skipIfLarger (default: true) → --skip-if-larger
 *   - pngquant/force (default: true) → --force
 *
 * @brief PngquantWorker::PngquantWorker
 * @param parent
 */

PngquantWorker::PngquantWorker(QObject *parent)
    : ImageWorker(parent) {}

void PngquantWorker::optimize(ImageTask *task) {
    QString src = task->imagePath;
    QString dst = task->optimizedPath;

    // Ensure destination directory exists
    QFileInfo dstInfo(dst);
    QDir dstDir = dstInfo.absoluteDir();
    if (!dstDir.exists()) {
        dstDir.mkpath(".");
    }

    // Remove existing destination file if force is enabled
    const QSettings &settings = Settings::instance().getSettings();
    bool force = settings.value("pngquant/force", true).toBool();

    if (force && QFile::exists(dst)) {
        QFile::remove(dst);
    }

    // Load settings
    int qualityMin = settings.value("pngquant/qualityMin", 65).toInt();
    int qualityMax = settings.value("pngquant/qualityMax", 80).toInt();
    int speed = settings.value("pngquant/speed", 4).toInt();
    bool enableDithering = settings.value("pngquant/enableDithering", true).toBool();
    int posterize = settings.value("pngquant/posterize", 0).toInt();
    bool stripMetadata = settings.value("pngquant/stripMetadata", true).toBool();
    bool skipIfLarger = settings.value("pngquant/skipIfLarger", true).toBool();

    // Build arguments
    QStringList args;

    // Quality range (min-max)
    args << "--quality=" + QString::number(qualityMin) + "-" + QString::number(qualityMax);

    // Speed setting (1-11)
    args << "--speed=" + QString::number(speed);

    // Output file
    args << "--output=" + dst;

    // Force overwrite
    if (force) {
        args << "--force";
    }

    // Strip metadata
    if (stripMetadata) {
        args << "--strip";
    }

    // Skip if larger
    if (skipIfLarger) {
        args << "--skip-if-larger";
    }

    // Disable dithering if requested
    if (!enableDithering) {
        args << "--nofs";
    }

    // Posterization
    if (posterize > 0) {
        args << "--posterize=" + QString::number(posterize);
    }

    // Separator and source file
    args << "--" << src;

    // Debug output
    qDebug() << "pngquant command:" << "pngquant" << args.join(" ");

    // Execute the optimization
    executeProcess("pngquant", args, task);
}



