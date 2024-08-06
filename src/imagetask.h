#ifndef IMAGETASK_H
#define IMAGETASK_H

#include <QMetaType>
#include <QString>

struct ImageTask {
  QString imagePath;     // Source image path
  QString optimizedPath; // Destination (optimized) image path

  enum Status { Pending, Queued, Processing, Completed, Error };

  Status taskStatus;

  QString statusToString() const {
    switch (taskStatus) {
    case Pending:
      return "Pending";
    case Queued:
      return "Queued";
    case Processing:
      return "Processing";
    case Completed:
      return "Completed";
    case Error:
      return "Error";
    default:
      return "Unknown";
    }
  }

  ImageTask(const QString &src, const QString &dst)
      : imagePath(src), optimizedPath(dst), taskStatus(Pending) {}
};

Q_DECLARE_METATYPE(ImageTask *)

#endif // IMAGETASK_H
