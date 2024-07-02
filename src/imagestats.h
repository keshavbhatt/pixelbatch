#ifndef IMAGESTATS_H
#define IMAGESTATS_H

#include <QFileInfo>
#include <QString>

class ImageStats {
public:
    ImageStats(const QString &originalFilePath, const QString &optimizedFilePath);

    qint64 getOriginalSize() const;
    qint64 getOptimizedSize() const;
    qint64 getSavings() const;
    double getCompressionPercentage() const;

private:
    QFileInfo originalFileInfo;
    QFileInfo optimizedFileInfo;
};

#endif // IMAGESTATS_H
