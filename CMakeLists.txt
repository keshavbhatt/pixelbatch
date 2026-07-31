cmake_minimum_required(VERSION 3.21)

project(pixelbatch
    VERSION 2.0
    DESCRIPTION "Batch image optimization tool"
    HOMEPAGE_URL "https://github.com/keshavbhatt/pixelbatch"
    LANGUAGES CXX
)

set(APP_NAME "PixelBatch")
set(APP_EXECUTABLE_NAME "pixelbatch")
set(APP_ID "com.ktechpit.pixelbatch")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Developed and shipped against Qt 6.11 (kde-qt6-core24-sdk / kf6-core24).
find_package(Qt6 6.4 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)

qt_standard_project_setup()

# Dev builds against the kde-qt6-core24-sdk snap on a non-Ubuntu host: some
# imported Qt targets export the SDK's generic /usr/include (Ubuntu glibc
# headers), which shadow the host toolchain's libc headers and break
# libstdc++. Strip that one directory from every imported target; the Qt
# headers proper live under .../include/<arch>/qt6 and are unaffected.
# Harmless in snapcraft builds (the dir then equals the real sysroot's).
if(CMAKE_PREFIX_PATH MATCHES "/snap/kde-qt6-core24-sdk/")
    get_property(_pixelbatch_imported DIRECTORY PROPERTY IMPORTED_TARGETS)
    foreach(_tgt IN LISTS _pixelbatch_imported)
        get_target_property(_incs ${_tgt} INTERFACE_INCLUDE_DIRECTORIES)
        if(_incs)
            set(_filtered "")
            foreach(_dir IN LISTS _incs)
                if(NOT _dir MATCHES "/snap/kde-qt6-core24-sdk/[^/]+/usr/include$")
                    list(APPEND _filtered "${_dir}")
                endif()
            endforeach()
            set_property(TARGET ${_tgt} PROPERTY
                INTERFACE_INCLUDE_DIRECTORIES "${_filtered}")
        endif()
    endforeach()
endif()

# Suppress qDebug output in non-debug builds.
add_compile_definitions(
    $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:QT_NO_DEBUG_OUTPUT>
)

# Build metadata shown in the About dialog's debug info.
execute_process(
    COMMAND git -C "${CMAKE_CURRENT_SOURCE_DIR}" rev-parse --short HEAD
    OUTPUT_VARIABLE GIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
execute_process(
    COMMAND git -C "${CMAKE_CURRENT_SOURCE_DIR}" rev-parse --abbrev-ref HEAD
    OUTPUT_VARIABLE GIT_BRANCH OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%dT%H:%M:%SUTC" UTC)

# Plain add_executable (not qt_add_executable): the app has no QML, and
# skipping Qt target finalization avoids running the SDK snap's
# qmlimportscanner, which cannot start on non-Ubuntu dev hosts.
add_executable(${APP_EXECUTABLE_NAME}
    src/main.cpp
    src/about.cpp
    src/about.h
    src/about.ui
    src/constants.cpp
    src/constants.h
    src/desktoputils.cpp
    src/desktoputils.h
    src/draggablelabel.cpp
    src/draggablelabel.h
    src/elideditemdelegate.cpp
    src/elideditemdelegate.h
    src/emptystatewidget.cpp
    src/emptystatewidget.h
    src/filehandler.cpp
    src/filehandler.h
    src/imagecomparisonwidget.cpp
    src/imagecomparisonwidget.h
    src/imagedetailpanel.cpp
    src/imagedetailpanel.h
    src/imageformatprefwidget.cpp
    src/imageformatprefwidget.h
    src/imageformatprefwidget.ui
    src/imagestats.cpp
    src/imagestats.h
    src/imagetask.h
    src/imagetype.h
    src/pixelbatch.cpp
    src/pixelbatch.h
    src/pixelbatch.ui
    src/preferenceswidget.cpp
    src/preferenceswidget.h
    src/preferenceswidget.ui
    src/settings.cpp
    src/settings.h
    src/taskactionwidget.cpp
    src/taskactionwidget.h
    src/taskactionwidget.ui
    src/taskwidget.cpp
    src/taskwidget.h
    src/taskwidgetoverlay.cpp
    src/taskwidgetoverlay.h
    src/thememanager.h
    src/OptimizerPrefWidgets/imageformatprefwidgetfactory.cpp
    src/OptimizerPrefWidgets/imageformatprefwidgetfactory.h
    src/OptimizerPrefWidgets/imageoptimizerprefwidget.cpp
    src/OptimizerPrefWidgets/imageoptimizerprefwidget.h
    src/OptimizerPrefWidgets/jpegoptimprefwidget.cpp
    src/OptimizerPrefWidgets/jpegoptimprefwidget.h
    src/OptimizerPrefWidgets/jpegoptimprefwidget.ui
    src/OptimizerPrefWidgets/pngquantprefwidget.cpp
    src/OptimizerPrefWidgets/pngquantprefwidget.h
    src/OptimizerPrefWidgets/pngquantprefwidget.ui
    src/OptimizerPrefWidgets/gifsicleprefwidget.cpp
    src/OptimizerPrefWidgets/gifsicleprefwidget.h
    src/OptimizerPrefWidgets/gifsicleprefwidget.ui
    src/OptimizerPrefWidgets/svgoprefwidget.cpp
    src/OptimizerPrefWidgets/svgoprefwidget.h
    src/OptimizerPrefWidgets/svgoprefwidget.ui
    src/worker/ImageWorker.h
    src/worker/imageoptimizer.cpp
    src/worker/imageoptimizer.h
    src/worker/imageworkerfactory.cpp
    src/worker/imageworkerfactory.h
    src/worker/jpegoptimworker.cpp
    src/worker/jpegoptimworker.h
    src/worker/pngquantworker.cpp
    src/worker/pngquantworker.h
    src/worker/gifsicleworker.cpp
    src/worker/gifsicleworker.h
    src/worker/svgoworker.cpp
    src/worker/svgoworker.h
    src/resources.qrc
)

# uic-generated headers include promoted-widget headers by bare name.
target_include_directories(${APP_EXECUTABLE_NAME} PRIVATE src)

target_compile_definitions(${APP_EXECUTABLE_NAME} PRIVATE
    APPLICATION_FULLNAME="${APP_NAME}"
    VERSIONSTR="${PROJECT_VERSION}"
    GIT_HASH="${GIT_HASH}"
    GIT_BRANCH="${GIT_BRANCH}"
    BUILD_TIMESTAMP="${BUILD_TIMESTAMP}"
)

target_link_libraries(${APP_EXECUTABLE_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Install rules
include(GNUInstallDirs)
install(TARGETS ${APP_EXECUTABLE_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

if(NOT WIN32)
    foreach(_size 16 32 64 128 256)
        install(FILES dist/linux/hicolor/${_size}x${_size}/apps/${APP_ID}.png
            DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${_size}x${_size}/apps
        )
    endforeach()
    install(FILES dist/linux/hicolor/scalable/apps/${APP_ID}.svg
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
    )
    install(FILES dist/linux/hicolor/symbolic/apps/${APP_ID}-symbolic.svg
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/symbolic/apps
    )
    install(FILES dist/linux/${APP_ID}.metainfo.xml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo
    )
    install(FILES dist/linux/${APP_ID}.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    )
endif()
