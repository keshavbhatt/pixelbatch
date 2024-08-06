#include "settings.h"

Settings &Settings::instance() {
  static Settings instance;
  return instance;
}

Settings::Settings(QObject *parent) : QObject(parent), settings() {}

QString Settings::getOptimizedPath() const {
  return settings
      .value(Constants::OPTIMIZED_PATH_KEY, Constants::DEFAULT_OPTIMIZED_PATH)
      .toString();
}

void Settings::setOptimizedPath(const QString &path) {
  settings.setValue(Constants::OPTIMIZED_PATH_KEY, path);
}

QString Settings::getOutputFilePrefix() const {
  return settings
      .value(Constants::OUTPUT_FILE_PREFIX_KEY,
             Constants::DEFAULT_OUTPUT_FILE_PREFIX)
      .toString();
}

void Settings::setOutputFilePrefix(const QString &prefix) {
  settings.setValue(Constants::OUTPUT_FILE_PREFIX_KEY, prefix);
}

QString Settings::getLastOpenedImageDirPath() const {
  return settings
      .value(Constants::INPUT_LAST_IMAGE_DIR_PATH_KEY,
             Constants::DEFAULT_INPUT_LAST_IMAGE_DIR_PATH)
      .toString();
}

void Settings::setLastOpenedImageDirPath(const QString &prefix) {
  settings.setValue(Constants::INPUT_LAST_IMAGE_DIR_PATH_KEY, prefix);
}

int Settings::getMaxConcurrentTasks() const {
  return settings
      .value(Constants::TASK_MAX_CONCURRENT_TASKS_KEY,
             Constants::DEFAULT_TASK_MAX_CONCURRENT_TASKS)
      .toInt();
}

void Settings::setMaxConcurrentTasks(const int &maxConcurrentTasks) {
  settings.setValue(Constants::TASK_MAX_CONCURRENT_TASKS_KEY,
                    maxConcurrentTasks);
}

bool Settings::getRememberOpenLastOpenedPath() const {
  return settings
      .value(Constants::INPUT_REMEMBER_LAST_IMAGE_DIR_PATH_KEY,
             Constants::DEFAULT_INPUT_REMEMBER_LAST_IMAGE_DIR_PATH)
      .toBool();
}

void Settings::setRememberOpenLastOpenedPath(const bool &remember) {
  settings.setValue(Constants::INPUT_REMEMBER_LAST_IMAGE_DIR_PATH_KEY,
                    remember);
}
