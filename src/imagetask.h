#ifndef IMAGETASK_H
#define IMAGETASK_H

#include <QString>

struct ImageTask {

    QString imagePath;       // Source image path
    QString optimizedPath;   // Destination (optimized) image path
    int rowIndex;            // Row index in TaskWidget table

    ImageTask(const QString &src, const QString &dst, int row);
};

#endif // IMAGETASK_H
