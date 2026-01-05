#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "settings.h"

#include <QFileDialog>
#include <QFileInfo>

#include <QObject>
#include <QTableWidget>
#include <QStandardPaths>

class FileHandler : public QObject {
  Q_OBJECT
public:
  explicit FileHandler(QObject *parent = nullptr);

public slots:
  void addFiles(const QString &dir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

signals:
  void batchAdditionStarting(); // Signal emitted before adding files
  void addFileToTable(const QString &fileName);
  void filesAdded(int count); // Signal to notify how many files were added

private:
  Settings &m_settings;

    void saveLasrOpenedImageDirPath(QStringList fileNames);
  void addFilesToTable(QStringList fileNames);
};

#endif // FILEHANDLER_H
