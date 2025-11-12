#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

class Constants {
public:
  static const QString OPTIMIZED_PATH_KEY;
  static const QString DEFAULT_OPTIMIZED_PATH;

  static const QString OUTPUT_FILE_PREFIX_KEY;
  static const QString DEFAULT_OUTPUT_FILE_PREFIX;

  static const QString INPUT_LAST_IMAGE_DIR_PATH_KEY;
  static const QString DEFAULT_INPUT_LAST_IMAGE_DIR_PATH;

  static const QString INPUT_REMEMBER_LAST_IMAGE_DIR_PATH_KEY;
  static const bool DEFAULT_INPUT_REMEMBER_LAST_IMAGE_DIR_PATH;

  static const QString TASK_MAX_CONCURRENT_TASKS_KEY;
  static const int DEFAULT_TASK_MAX_CONCURRENT_TASKS;

  static const QString APPEARANCE_THEME_KEY;
  static const QString DEFAULT_APPEARANCE_THEME;

  static const QString APPEARANCE_STYLE_KEY;
  static const QString DEFAULT_APPEARANCE_STYLE;

  static const QString APPEARANCE_USE_SYSTEM_THEME_KEY;
  static const bool DEFAULT_APPEARANCE_USE_SYSTEM_THEME;

  // Application metadata
  static const QString APP_DESCRIPTION;
  static const QString APP_LICENSE;
  static const QString APP_AUTHOR_NAME;

  // URLs
  static const QString URL_GITHUB_REPO;
  static const QString URL_GITHUB_ISSUES;
  static const QString URL_GITHUB_AUTHOR;
  static const QString URL_WEBSITE;
  static const QString URL_DONATE;

private:
  Constants() = default;
};

#endif // CONSTANTS_H
