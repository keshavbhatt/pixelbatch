#ifndef DESKTOPUTILS_H
#define DESKTOPUTILS_H

#include <QString>

class DesktopUtils {
public:
  // Open URL (web links, file:// URLs, etc.)
  static bool openUrl(const QString &url);

  // Open file in default application
  static bool openFile(const QString &filePath);

  // Open folder in file manager (selects file if provided)
  static bool openFolderAndSelectFile(const QString &filePath);

  // Open folder in file manager
  static bool openFolder(const QString &folderPath);

private:
  DesktopUtils() = default; // Utility class, no instances

  // Try to open using xdg-open (works better in snap)
  static bool openWithXdgOpen(const QString &target);

  // Fallback to QDesktopServices
  static bool openWithQDesktopServices(const QString &url);
};

#endif // DESKTOPUTILS_H
