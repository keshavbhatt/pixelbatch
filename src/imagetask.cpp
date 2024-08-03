#include "imagetask.h"

ImageTask::ImageTask(const QString &src, const QString &dst, int row)
    : imagePath(src), optimizedPath(dst), rowIndex(row), taskStatus(Pending) {}
