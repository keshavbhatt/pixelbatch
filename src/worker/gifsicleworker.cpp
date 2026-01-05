#include "gifsicleworker.h"
#include "settings.h"
#include <QProcess>
#include <QFileInfo>
#include <QFile>
#include <QDir>

/**
 * https://www.lcdf.org/gifsicle/
 * Based on gifsicle 1.95
 *
 * GIF Optimization Worker using gifsicle
 *
 * gifsicle is a powerful command-line tool for creating, editing, and optimizing
 * GIF images and animations. It supports both lossless and lossy compression.
 *
 * Key Features:
 * - Lossless optimization (levels 1-3)
 * - Lossy compression with quality control (1-200)
 * - Color palette reduction (2-256 colors)
 * - Dithering for smooth gradients
 * - Multi-threading support
 * - Animation-aware optimization
 *
 * Settings Mapping:
 *
 * Optimization Level (gifsicle/optimizationLevel, default: 2):
 *   - 1 = Basic optimization (-O1)
 *   - 2 = Normal optimization (-O2)
 *   - 3 = Maximum optimization (-O3)
 *   → -O1, -O2, or -O3
 *
 * Compression Type (gifsicle/compressionType, default: 0):
 *   - 0 = Lossless (no quality loss)
 *   - 1 = Lossy (allows quality trade-off)
 *   → --lossy=N (when type=1)
 *
 * Lossy Level (gifsicle/lossyLevel, default: 80):
 *   - Range: 1-200
 *   - Lower = better quality, larger file
 *   - Higher = more compression, possible artifacts
 *   → --lossy=N
 *
 * Color Reduction (gifsicle/reduceColors, default: false):
 *   - When enabled, reduces palette to specified count
 *   - gifsicle/colorCount: 2-256 (default: 256)
 *   - gifsicle/colorMethod: 0=diversity, 1=blend-diversity, 2=median-cut
 *   → --colors=N --color-method=METHOD
 *
 * Dithering (gifsicle/enableDithering, default: true):
 *   - Apply dithering when reducing colors
 *   - Improves gradient appearance
 *   → -f, --dither
 *
 * Additional Options:
 *   - gifsicle/cropTransparency (default: true) → --crop-transparency
 *   - gifsicle/interlace (default: false) → -i, --interlace
 *   - gifsicle/threads (default: 4) → -j, --threads=N
 *
 * Example Commands:
 *
 * Lossless Level 3:
 *   gifsicle -O3 --crop-transparency -j4 -o output.gif input.gif
 *
 * Lossy Level 3 with lossy=80:
 *   gifsicle -O3 --lossy=80 --crop-transparency -j4 -o output.gif input.gif
 *
 * Lossy + Color Reduction:
 *   gifsicle -O3 --lossy=80 --colors=256 -f --crop-transparency -j4 -o output.gif input.gif
 *
 * @brief GifsicleWorker::GifsicleWorker
 * @param parent
 */

GifsicleWorker::GifsicleWorker(QObject *parent)
    : ImageWorker(parent) {}

void GifsicleWorker::optimize(ImageTask *task) {
    QString src = task->imagePath;
    QString dst = task->optimizedPath;

    // Ensure destination directory exists
    QFileInfo dstInfo(dst);
    QDir dstDir = dstInfo.absoluteDir();
    if (!dstDir.exists()) {
        dstDir.mkpath(".");
    }

    // Remove existing destination file
    if (QFile::exists(dst)) {
        QFile::remove(dst);
    }

    // Load settings (using getSetting which checks custom settings first, then falls back to global)
    int optimizationLevel = getSetting("gifsicle/optimizationLevel", 2).toInt();
    int compressionType = getSetting("gifsicle/compressionType", 0).toInt();
    int lossyLevel = getSetting("gifsicle/lossyLevel", 80).toInt();
    bool reduceColors = getSetting("gifsicle/reduceColors", false).toBool();
    int colorCount = getSetting("gifsicle/colorCount", 256).toInt();
    int colorMethod = getSetting("gifsicle/colorMethod", 0).toInt();
    bool enableDithering = getSetting("gifsicle/enableDithering", true).toBool();
    bool cropTransparency = getSetting("gifsicle/cropTransparency", true).toBool();
    bool interlace = getSetting("gifsicle/interlace", false).toBool();
    int threads = getSetting("gifsicle/threads", 4).toInt();

    // Build arguments
    QStringList args;

    // Optimization level
    args << "-O" + QString::number(optimizationLevel);

    // Lossy compression
    if (compressionType == 1) {
        args << "--lossy=" + QString::number(lossyLevel);
    }

    // Color reduction
    if (reduceColors) {
        args << "--colors=" + QString::number(colorCount);

        // Color method
        QString method;
        switch (colorMethod) {
            case 1:
                method = "blend-diversity";
                break;
            case 2:
                method = "median-cut";
                break;
            case 0:
            default:
                method = "diversity";
                break;
        }
        args << "--color-method=" + method;
    }

    // Dithering (only when reducing colors)
    if (reduceColors && enableDithering) {
        args << "-f";  // --dither
    }

    // Crop transparency
    if (cropTransparency) {
        args << "--crop-transparency";
    }

    // Interlacing
    if (interlace) {
        args << "-i";  // --interlace
    }

    // Multi-threading
    if (threads > 1) {
        args << "-j" + QString::number(threads);
    }

    // Output file
    args << "-o" << dst;

    // Input file
    args << src;

    // Debug output
    qDebug() << "gifsicle command:" << "gifsicle" << args.join(" ");

    // Execute the optimization
    executeProcess("gifsicle", args, task);
}

