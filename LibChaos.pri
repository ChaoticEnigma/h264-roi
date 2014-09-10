#TARGET = chaos
TEMPLATE = lib
CONFIG += staticlib console
QT -= core gui widgets

# LibChaos
HEADERS += \
    $$PWD/chaos/zerror.h \
    $$PWD/chaos/zautobuffer.h \
    $$PWD/chaos/zautobufferunit.h \
    $$PWD/chaos/zlog.h \
    $$PWD/chaos/zlogworker.h \
    $$PWD/chaos/zmutex.h \
    $$PWD/chaos/zsocket.h \
    $$PWD/chaos/zthread.h \
    $$PWD/chaos/ztypes.h \
    $$PWD/chaos/ztime.h \
    $$PWD/chaos/zclock.h \
    $$PWD/chaos/zret.h \
    \
    $$PWD/chaos/data/zarray.h \
    $$PWD/chaos/data/zarray2.h \
    $$PWD/chaos/data/zassoc.h \
    $$PWD/chaos/data/zassoc2.h \
    $$PWD/chaos/data/zbinary.h \
    $$PWD/chaos/data/zdata.h \
    $$PWD/chaos/data/zjson.h \
    $$PWD/chaos/data/zmap.h \
    $$PWD/chaos/data/zpath.h \
    $$PWD/chaos/data/zstring.h \
    $$PWD/chaos/data/zxml.h \
    \
    $$PWD/chaos/file/database.h \
    $$PWD/chaos/file/zbitmap.h \
    $$PWD/chaos/file/zfile.h \
    \
    $$PWD/chaos/lib/sqlite3.h \
    \
    $$PWD/chaos/net/zsocket.h \
    $$PWD/chaos/net/zconnection.h \
    $$PWD/chaos/net/zstreamsocket.h \
    $$PWD/chaos/net/zdatagramsocket.h \
    $$PWD/chaos/net/zstreamconnection.h \
    $$PWD/chaos/net/zaddress.h \
    \
    $$PWD/test/test.h \
    \
    $$PWD/ui/zui.h \
    $$PWD/ui/base.h \
    $$PWD/ui/window.h \
    $$PWD/ui/control.h \
    $$PWD/ui/button.h \
    $$PWD/ui/edit.h \
    $$PWD/ui/font.h \
    $$PWD/ui/table.h \
    $$PWD/ui/progress.h \
    $$PWD/ui/align.h \
    \
    $$PWD/bbb/zbbb.h \
    \
    $$PWD/chaos/zcondition.h \
    chaos/data/znumber.h \
    $$PWD/chaos/file/zpng.h \
    $$PWD/chaos/file/zimage.h \
    $$PWD/chaos/file/zbmp.h

SOURCES += \
    $$PWD/chaos/zautobuffer.cpp \
    $$PWD/chaos/zerror.cpp \
    $$PWD/chaos/zlog.cpp \
    $$PWD/chaos/zlogworker.cpp \
    $$PWD/chaos/zsocket.cpp \
    $$PWD/chaos/zthread.cpp \
    $$PWD/chaos/ztime.cpp \
    $$PWD/chaos/zclock.cpp \
    \
    $$PWD/chaos/data/zbinary.cpp \
    $$PWD/chaos/data/zjson.cpp \
    $$PWD/chaos/data/zpath.cpp \
    $$PWD/chaos/data/zstring.cpp \
    $$PWD/chaos/data/zstring_encode.cpp \
    $$PWD/chaos/data/zxml.cpp \
    \
    $$PWD/chaos/file/database.cpp \
    $$PWD/chaos/file/zfile.cpp \
    \
    $$PWD/chaos/lib/sqlite3.c \
    \
    $$PWD/chaos/net/zsocket.cpp \
    $$PWD/chaos/net/zconnection.cpp \
    $$PWD/chaos/net/zstreamsocket.cpp \
    $$PWD/chaos/net/zdatagramsocket.cpp \
    $$PWD/chaos/net/zstreamconnection.cpp \
    $$PWD/chaos/net/zaddress.cpp \
    \
    $$PWD/test/libchaos_main.cpp \
    $$PWD/test/autobuffer_test.cpp \
    $$PWD/test/thread_test.cpp \
    $$PWD/test/string_test.cpp \
    $$PWD/test/array_test.cpp \
    $$PWD/test/file_test.cpp \
    $$PWD/test/error_test.cpp \
    $$PWD/test/socket_test.cpp \
    $$PWD/test/json_test.cpp \
    $$PWD/test/socket2_test.cpp \
    \
    $$PWD/ui/win32/window_win32.cpp \
    $$PWD/ui/win32/windowproc_win32.cpp \
    $$PWD/ui/win32/control_win32.cpp \
    $$PWD/ui/win32/button_win32.cpp \
    $$PWD/ui/win32/edit_win32.cpp \
    $$PWD/ui/win32/font_win32.cpp \
    $$PWD/ui/win32/table_win32.cpp \
    $$PWD/ui/win32/progress_win32.cpp \
    $$PWD/ui/win32/align_win32.cpp \
    \
    $$PWD/bbb/io.cpp \
    \
    $$PWD/util/extensionfixer.cpp \
    $$PWD/util/audiobooksorter.cpp \
    $$PWD/util/imagedownloader.cpp \
    $$PWD/util/duplicatefinder.cpp \
    chaos/data/znumber.cpp \
    test/number_test.cpp \
    $$PWD/chaos/file/zpng.cpp \
    $$PWD/test/png_test.cpp \
    $$PWD/chaos/file/zbmp.cpp

OTHER_FILES += \
    $$PWD/CMakeLists.txt \
    $$PWD/ui/CMakeLists.txt \
    $$PWD/bbb/CMakeLists.txt \
    $$PWD/chaos/CMakeLists.txt \
    $$PWD/test/CMakeLists.txt \
    $$PWD/configure_build.cmake \
    $$PWD/todo.txt \
    $$PWD/LibChaos_Todo_Ideas.txt \
    $$PWD/util/CMakeLists.txt \
    $$PWD/source_hash.cmake \
    $$PWD/hash.sh

INCLUDEPATH += \
    $$PWD \
    $$PWD/chaos \
    $$PWD/chaos/data \
    $$PWD/chaos/file \
    $$PWD/chaos/lib \
    $$PWD/chaos/net \
    $$PWD/test \
    $$PWD/ui \
    $$PWD/bbb \
    $$PWD/util

# Build
#DEFINES += ZLOG_STD_MUTEX
QMAKE_CXXFLAGS += -Wall -Wextra -pedantic -ansi -std=c++11 -std=c++0x
LIBS += pthread

win32:DEFINES += _LIBCHAOS_PLATFORM_WINDOWS _LIBCHAOS_COMPILER_MINGW
else:DEFINES += _LIBCHAOS_PLATFORM_LINUX _LIBCHAOS_COMPILER_GCC

debug:DEFINES += _LIBCHAOS_BUILD_DEBUG
#else release:DEFINES += _LIBCHAOS_BUILD_RELEASE
else:DEFINES += _LIBCHAOS_BUILD_NORMAL
