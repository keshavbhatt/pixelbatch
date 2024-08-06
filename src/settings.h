#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

#include "constants.h"

class Settings : public QObject {
  Q_OBJECT
public:
  static Settings &instance();

  QString getOptimizedPath() const;
  void setOptimizedPath(const QString &path);

  QString getOutputFilePrefix() const;
  void setOutputFilePrefix(const QString &prefix);
  QString getLastOpenedImageDirPath() const;
  void setLastOpenedImageDirPath(const QString &prefix);
  int getMaxConcurrentTasks() const;

private:
  Settings(QObject *parent = nullptr);            // Private constructor
  Settings(const Settings &) = delete;            // Disable copy constructor
  Settings &operator=(const Settings &) = delete; // Disable assignment operator

  QSettings settings;
};

#endif // SETTINGS_H
