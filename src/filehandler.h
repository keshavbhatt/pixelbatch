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
  void addFileToTable(const QString &fileName);

private:
  Settings &m_settings;

    void saveLasrOpenedImageDirPath(QStringList fileNames);
  void addFilesToTable(QStringList fileNames);
};

#endif // FILEHANDLER_H
