#include "filehandler.h"

#include "worker/imageworkerfactory.h"

FileHandler::FileHandler(QObject *parent)
    : QObject(parent), m_settings(Settings::instance()) {}

// Dialog name filter covering every format the registered optimizers
// support. Native file dialogs match globs case-sensitively on Linux, so
// include upper-case variants too (issue #9: "jpg != JPG").
QString FileHandler::supportedImagesFilter() {
  QStringList patterns;
  const auto optimizers =
      ImageWorkerFactory::instance().getRegisteredImageOptimizers();
  for (const ImageOptimizer &optimizer : optimizers) {
    const auto formats = optimizer.getSupportedFormats();
    for (const QString &ext : formats) {
      patterns << "*." + ext.toLower() << "*." + ext.toUpper();
    }
  }
  patterns.removeDuplicates();
  return tr("Images (%1)").arg(patterns.join(QLatin1Char(' ')));
}

void FileHandler::addFiles(const QString &dir) {
  QStringList fileNames = QFileDialog::getOpenFileNames(
      nullptr, tr("Select Images"), dir, supportedImagesFilter());

  if (fileNames.isEmpty() == false ) {
    saveLasrOpenedImageDirPath(fileNames);
    addFilesToTable(fileNames);
  }
}

void FileHandler::addFilesToTable(QStringList fileNames)
{
    // Emit signal before starting to add files
    emit batchAdditionStarting();

    for (const QString &fileName : qAsConst(fileNames)) {
        emit addFileToTable(fileName);
    }

    // Emit signal with count of files to be added
    emit filesAdded(fileNames.count());
}


void FileHandler::saveLasrOpenedImageDirPath(QStringList fileNames) {
  QFileInfo fileInfo(fileNames.at(0));
  if (fileInfo.isFile()) {
    m_settings.setLastOpenedImageDirPath(fileInfo.dir().absolutePath());
  }
}
