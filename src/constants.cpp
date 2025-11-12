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

const QString Constants::APPEARANCE_THEME_KEY = "appearance/theme";
const QString Constants::DEFAULT_APPEARANCE_THEME = "Light";

const QString Constants::APPEARANCE_STYLE_KEY = "appearance/style";
const QString Constants::DEFAULT_APPEARANCE_STYLE = "Fusion";

const QString Constants::APPEARANCE_USE_SYSTEM_THEME_KEY =
    "appearance/useSystemTheme";
const bool Constants::DEFAULT_APPEARANCE_USE_SYSTEM_THEME = true;

// Application metadata
const QString Constants::APP_DESCRIPTION =
    "Batch Image Optimization Tool for JPEG, PNG, GIF, and SVG";
const QString Constants::APP_LICENSE = "GPL-3.0-or-later";
const QString Constants::APP_AUTHOR_NAME = "Keshav Bhatt and Contributors";

// URLs
const QString Constants::URL_GITHUB_REPO =
    "https://github.com/keshavbhatt/pixelbatch";
const QString Constants::URL_GITHUB_ISSUES =
    "https://github.com/keshavbhatt/pixelbatch/issues";
const QString Constants::URL_GITHUB_AUTHOR = "https://github.com/keshavbhatt";
const QString Constants::URL_WEBSITE = "https://ktechpit.com/USS/public";
const QString Constants::URL_DONATE =
    "https://www.paypal.com/paypalme/keshavnrj/"
    "15USD?note=Donation%20for%20PixelBatch%20Application";
