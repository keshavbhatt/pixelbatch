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
  static const QString DEFAULT_INPUT_LAST_IMAGE_DIR_PATH_KEY;

private:
  Constants() = default;
};

#endif // CONSTANTS_H
