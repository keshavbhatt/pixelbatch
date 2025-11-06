#include "desktoputils.h"

#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QThread>
#include <QUrl>

bool DesktopUtils::openUrl(const QString &url) {
  // Try xdg-open first (works better in snap confinement)
  if (openWithXdgOpen(url)) {
    return true;
  }

  // Fallback to QDesktopServices
  return openWithQDesktopServices(url);
}

bool DesktopUtils::openFile(const QString &filePath) {
  QFileInfo fileInfo(filePath);
  if (!fileInfo.exists() || !fileInfo.isFile()) {
    qWarning() << "File does not exist:" << filePath;
    return false;
  }

  QString absolutePath = fileInfo.absoluteFilePath();

  // Try xdg-open first
  if (openWithXdgOpen(absolutePath)) {
    return true;
  }

  // Fallback to QDesktopServices with file:// URL
  return openWithQDesktopServices("file://" + absolutePath);
}

bool DesktopUtils::openFolderAndSelectFile(const QString &filePath) {
  QFileInfo fileInfo(filePath);
  if (!fileInfo.exists()) {
    qWarning() << "File does not exist:" << filePath;
    return false;
  }

  QString absolutePath = fileInfo.absoluteFilePath();
  QString folderPath = fileInfo.absolutePath();

  // Try to select file in file manager using various methods

  // Method 1: Try dbus call to file manager (works on many Linux DEs)
  QProcess dbusProcess;
  dbusProcess.start("dbus-send", QStringList()
                                     << "--session"
                                     << "--print-reply"
                                     << "--dest=org.freedesktop.FileManager1"
                                     << "/org/freedesktop/FileManager1"
                                     << "org.freedesktop.FileManager1.ShowItems"
                                     << "array:string:file://" + absolutePath
                                     << "string:");

  if (dbusProcess.waitForFinished(1000) && dbusProcess.exitCode() == 0) {
    return true;
  }

  // Method 2: Try xdg-open on the folder (won't select file but will open
  // folder)
  if (openWithXdgOpen(folderPath)) {
    return true;
  }

  // Fallback: Open folder with QDesktopServices
  return openWithQDesktopServices("file://" + folderPath);
}

bool DesktopUtils::openFolder(const QString &folderPath) {
  QFileInfo dirInfo(folderPath);
  if (!dirInfo.exists() || !dirInfo.isDir()) {
    qWarning() << "Folder does not exist:" << folderPath;
    return false;
  }

  QString absolutePath = dirInfo.absoluteFilePath();

  // Try xdg-open first
  if (openWithXdgOpen(absolutePath)) {
    return true;
  }

  // Fallback to QDesktopServices
  return openWithQDesktopServices("file://" + absolutePath);
}

bool DesktopUtils::openWithXdgOpen(const QString &target) {
  // Check if xdg-open is available
  QProcess checkProcess;
  checkProcess.start("which", QStringList() << "xdg-open");
  if (!checkProcess.waitForFinished(1000) || checkProcess.exitCode() != 0) {
    qDebug() << "xdg-open not found";
    return false;
  }

  // Use xdg-open to open the target
  QProcess process;
  process.startDetached("xdg-open", QStringList() << target);

  // Give it a moment to start
  QThread::msleep(100);

  // Assume success if process started (xdg-open returns immediately)
  return true;
}

bool DesktopUtils::openWithQDesktopServices(const QString &url) {
  QUrl qurl(url);

  // If it's not already a URL scheme, treat as file path
  if (qurl.scheme().isEmpty()) {
    QFileInfo info(url);
    if (info.exists()) {
      qurl = QUrl::fromLocalFile(info.absoluteFilePath());
    }
  }

  bool result = QDesktopServices::openUrl(qurl);
  if (!result) {
    qWarning() << "QDesktopServices::openUrl failed for:" << qurl.toString();
  }

  return result;
}
