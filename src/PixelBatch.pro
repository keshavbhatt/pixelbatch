QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

#CONFIG += sanitizer
#CONFIG += sanitize_address
#CONFIG += sanitize_memory

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = pixelbatch
TEMPLATE = app

# No debug output in release mode
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# Print if this is a debug or release build
CONFIG(debug, debug|release) {
    message("This is a debug build")
} else {
    message("This is a release build")
}

# Set program version
VERSION = 1.0
DEFINES += VERSIONSTR=\\\"$${VERSION}\\\"

APPLICATION_FULLNAME = PixelBatch
DEFINES += APPLICATION_FULLNAME=\\\"$${APPLICATION_FULLNAME}\\\"

# Define git info
GIT_HASH="\\\"$$system(git -C \""$$_PRO_FILE_PWD_"\" rev-parse --short HEAD)\\\""
GIT_BRANCH="\\\"$$system(git -C \""$$_PRO_FILE_PWD_"\" rev-parse --abbrev-ref HEAD)\\\""
BUILD_TIMESTAMP="\\\"$$system(date -u +\""%Y-%m-%dT%H:%M:%SUTC\"")\\\""
DEFINES += GIT_HASH=$$GIT_HASH GIT_BRANCH=$$GIT_BRANCH BUILD_TIMESTAMP=$$BUILD_TIMESTAMP

SOURCES += \
    elideditemdelegate.cpp \
    main.cpp \
    pixelbatch.cpp \
    taskwidget.cpp

HEADERS += \
    elideditemdelegate.h \
    pixelbatch.h \
    taskwidget.h

FORMS += \
    pixelbatch.ui

# Default rules for deployment
isEmpty(PREFIX){
 PREFIX = /usr
}

message("Installation prefix: $$PREFIX")

BINDIR  = $$PREFIX/bin
DATADIR = $$PREFIX/share

target.path = $$BINDIR

# Install Icons
icon16.path = $$PREFIX/share/icons/hicolor/16x16/apps/
icon16.files = ../dist/linux/hicolor/16x16/apps/com.ktechpit.pixelbatch.png

icon32.path = $$PREFIX/share/icons/hicolor/32x32/apps/
icon32.files = ../dist/linux/hicolor/32x32/apps/com.ktechpit.pixelbatch.png

icon64.path = $$PREFIX/share/icons/hicolor/64x64/apps/
icon64.files = ../dist/linux/hicolor/64x64/apps/com.ktechpit.pixelbatch.png

icon128.path = $$PREFIX/share/icons/hicolor/128x128/apps/
icon128.files = ../dist/linux/hicolor/128x128/apps/com.ktechpit.pixelbatch.png

icon256.path = $$PREFIX/share/icons/hicolor/256x256/apps/
icon256.files = ../dist/linux/hicolor/256x256/apps/com.ktechpit.pixelbatch.png

iconscalable.path = $$PREFIX/share/icons/hicolor/scalable/apps/
iconscalable.files = ../dist/linux/hicolor/scalable/apps/com.ktechpit.pixelbatch.svg

iconsymbolic.path = $$PREFIX/share/icons/hicolor/symbolic/apps/
iconsymbolic.files = ../dist/linux/hicolor/symbolic/apps/com.ktechpit.pixelbatch-symbolic.svg

# Install appstream meta data
appstream.path = $$PREFIX/share/metainfo/
appstream.files = ../dist/linux/com.ktechpit.pixelbatch.metainfo.xml

# Install desktop file
desktop.path  = $$DATADIR/applications/
desktop.files = ../dist/linux/com.ktechpit.pixelbatch.desktop

unix:INSTALLS += target icon16 icon32 icon64 icon128 icon256
unix:INSTALLS += iconscalable iconsymbolic appstream desktop
