#ifndef IMAGETASK_H
#define IMAGETASK_H

#include <QMetaType>
#include <QString>

struct ImageTask {
  QString imagePath;     // Source image path
  QString optimizedPath; // Destination (optimized) image path
  int rowIndex;          // Row index in TaskWidget table

  enum Status { Pending, Processing, Completed, Error };

  Status taskStatus;

  QString statusToString() const {
    switch (taskStatus) {
    case Pending:
      return "Pending";
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

  ImageTask(const QString &src, const QString &dst, int row);
};

Q_DECLARE_METATYPE(ImageTask *)

#endif // IMAGETASK_H
