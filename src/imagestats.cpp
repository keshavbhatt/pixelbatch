#include "imagestats.h"

ImageStats::ImageStats(const QString &originalFilePath, const QString &optimizedFilePath)
    : originalFileInfo(originalFilePath), optimizedFileInfo(optimizedFilePath) {}

qint64 ImageStats::getOriginalSize() const {
    return originalFileInfo.size();
}

qint64 ImageStats::getOptimizedSize() const {
    return optimizedFileInfo.size();
}

qint64 ImageStats::getSavings() const {
    return getOriginalSize() - getOptimizedSize();
}

double ImageStats::getCompressionPercentage() const {
    if (getOriginalSize() == 0) {
        return 0.0;
    }
    return 100.0 * (1.0 - static_cast<double>(getOptimizedSize()) / static_cast<double>(getOriginalSize()));
}
