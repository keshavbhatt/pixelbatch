#include "settings.h"


Settings& Settings::instance() {
    static Settings instance;
    return instance;
}

Settings::Settings(QObject *parent)
    : QObject(parent),
    settings() {}

QString Settings::getOptimizedPath() const {
    return settings.value(Constants::OPTIMIZED_PATH_KEY, Constants::DEFAULT_OPTIMIZED_PATH).toString();
}

void Settings::setOptimizedPath(const QString &path) {
    settings.setValue(Constants::OPTIMIZED_PATH_KEY, path);
}

QString Settings::getOutputFilePrefix() const {
    return settings.value(Constants::OUTPUT_FILE_PREFIX_KEY, Constants::DEFAULT_OUTPUT_FILE_PREFIX).toString();
}

void Settings::setOutputFilePrefix(const QString &prefix) {
    settings.setValue(Constants::OUTPUT_FILE_PREFIX_KEY, prefix);
}
