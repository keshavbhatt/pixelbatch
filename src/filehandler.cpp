#include "filehandler.h"

FileHandler::FileHandler(QObject *parent) : QObject(parent) {}

void FileHandler::addFiles() {
  QStringList fileNames =
      QFileDialog::getOpenFileNames(nullptr, tr("Select Images"), QString(),
                                    tr("Images (*.png *.jpg *.jpeg *.gif)"));

  if (!fileNames.isEmpty()) {
    for (const QString &fileName : fileNames) {
      emit addFileToTable(fileName);
    }
  }
}
