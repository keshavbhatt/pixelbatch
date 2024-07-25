#include "filehandler.h"

FileHandler::FileHandler(QObject *parent)
    : QObject(parent), m_settings(Settings::instance()) {}


void FileHandler::addFiles(const QString &dir) {
  QStringList fileNames =
      QFileDialog::getOpenFileNames(nullptr, tr("Select Images"), dir,
                                    tr("Images (*.png *.jpg *.jpeg *.gif)"));

  if (fileNames.isEmpty() == false ) {
    saveLasrOpenedImageDirPath(fileNames);
    addFilesToTable(fileNames);
  }
}

void FileHandler::addFilesToTable(QStringList fileNames)
{
    for (const QString &fileName : qAsConst(fileNames)) {
        emit addFileToTable(fileName);
    }
}


void FileHandler::saveLasrOpenedImageDirPath(QStringList fileNames) {
  QFileInfo fileInfo(fileNames.at(0));
  if (fileInfo.isFile()) {
    m_settings.setLastOpenedImageDirPath(fileInfo.dir().absolutePath());
  }
}
