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
  static const QString TASK_MAX_CONCURRENT_TASKS_KEY;
  static const int DEFAULT_TASK_MAX_CONCURRENT_TASKS;

private:
  Constants() = default;
};

#endif // CONSTANTS_H
