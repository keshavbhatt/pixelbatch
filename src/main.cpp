#include "pixelbatch.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>
#include <QTimer>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QApplication::setApplicationName(APPLICATION_FULLNAME);
  QApplication::setDesktopFileName("com.ktechpit.pixelbatch");
  QApplication::setOrganizationDomain("com.ktechpit");
  QApplication::setOrganizationName("org.keshavnrj.ubuntu");
  QApplication::setApplicationVersion(VERSIONSTR);

  qRegisterMetaType<ImageTask *>("ImageTask*");

  // Parse command line arguments
  QCommandLineParser parser;
  parser.setApplicationDescription("Batch Image Optimization Tool");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("files", "Image files or folders to optimize", "[files...]");
  parser.process(a);

  PixelBatch w;
  w.show();

  // Process command-line arguments (files/folders)
  QStringList args = parser.positionalArguments();
  if (!args.isEmpty()) {
    // Delay processing to allow UI to initialize
    QTimer::singleShot(100, [&w, args]() {
      QStringList filesToAdd;

      for (const QString &arg : args) {
        QFileInfo fileInfo(arg);

        if (fileInfo.isFile()) {
          // Single file
          filesToAdd.append(fileInfo.absoluteFilePath());
        } else if (fileInfo.isDir()) {
          // Directory - find all image files
          QDir dir(fileInfo.absoluteFilePath());
          QStringList nameFilters;
          nameFilters << "*.jpg" << "*.jpeg" << "*.JPG" << "*.JPEG"
                      << "*.png" << "*.PNG"
                      << "*.gif" << "*.GIF"
                      << "*.svg" << "*.SVG";

          QFileInfoList files = dir.entryInfoList(nameFilters, QDir::Files);
          for (const QFileInfo &file : files) {
            filesToAdd.append(file.absoluteFilePath());
          }
        }
      }

      // Add files to the task widget
      for (const QString &file : filesToAdd) {
        w.addFileFromCommandLine(file);
      }
    });
  }

  return a.exec();
}
