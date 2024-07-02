#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QFileDialog>
#include <QFileInfo>

#include <QObject>
#include <QTableWidget>

class FileHandler : public QObject {
  Q_OBJECT
public:
  explicit FileHandler(QObject *parent = nullptr);

public slots:
  void addFiles();

signals:
  void addFileToTable(const QString &fileName);
};

#endif // FILEHANDLER_H
