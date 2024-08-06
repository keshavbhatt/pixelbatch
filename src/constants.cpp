#include "constants.h"

#include <QStandardPaths>

const QString Constants::OPTIMIZED_PATH_KEY = "paths/optimizedPath";
const QString Constants::DEFAULT_OPTIMIZED_PATH = "/tmp/d/";

const QString Constants::OUTPUT_FILE_PREFIX_KEY = "file/prefix";
const QString Constants::DEFAULT_OUTPUT_FILE_PREFIX = "optimized_";

const QString Constants::INPUT_LAST_IMAGE_DIR_PATH_KEY =
    "file/last_opened_image_dir_path";
const QString Constants::DEFAULT_INPUT_LAST_IMAGE_DIR_PATH =
    QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);

const QString Constants::INPUT_REMEMBER_LAST_IMAGE_DIR_PATH_KEY =
    "file/remember_last_opened_image_dir_path";
const bool Constants::DEFAULT_INPUT_REMEMBER_LAST_IMAGE_DIR_PATH = true;

const QString Constants::TASK_MAX_CONCURRENT_TASKS_KEY =
    "task/max_concurrent_tasks";
const int Constants::DEFAULT_TASK_MAX_CONCURRENT_TASKS = 1;
