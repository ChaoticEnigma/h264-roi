TARGET = chaos
TEMPLATE = lib
CONFIG += staticlib console
QT -= core gui widgets

# LibChaos
HEADERS += \
    $$PWD/src/database.h \
    $$PWD/src/zarray.h \
    $$PWD/src/zarray2.h \
    $$PWD/src/zassoc.h \
    $$PWD/src/zassoc2.h \
    $$PWD/src/zautobuffer.h \
    $$PWD/src/zautobufferunit.h \
    $$PWD/src/zbinary.h \
    $$PWD/src/zerror.h \
    $$PWD/src/zfile.h \
    $$PWD/src/zjson.h \
    $$PWD/src/zlog.h \
    $$PWD/src/zlogworker.h \
    $$PWD/src/zmap.h \
    $$PWD/src/zmutex.h \
    $$PWD/src/zpath.h \
    $$PWD/src/zsocket.h \
    $$PWD/src/zstring.h \
    $$PWD/src/zthread.h \
    $$PWD/src/ztypes.h \
    $$PWD/src/zxml.h \
    $$PWD/lib/sqlite3.h \
    $$PWD/test/test.h \
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
    $$PWD/bbb/zbbb.h

SOURCES += \
    $$PWD/src/database.cpp \
    $$PWD/src/zautobuffer.cpp \
    $$PWD/src/zbinary.cpp \
    $$PWD/src/zerror.cpp \
    $$PWD/src/zfile.cpp \
    $$PWD/src/zjson.cpp \
    $$PWD/src/zlog.cpp \
    $$PWD/src/zlogworker.cpp \
    $$PWD/src/zpath.cpp \
    $$PWD/src/zsocket.cpp \
    $$PWD/src/zstring.cpp \
    $$PWD/src/zstring_encode.cpp \
    $$PWD/src/zthread.cpp \
    $$PWD/src/zxml.cpp \
    $$PWD/lib/sqlite3.c \
    $$PWD/test/libchaos-main.cpp \
    $$PWD/test/autobuffer_test.cpp \
    $$PWD/test/thread_test.cpp \
    $$PWD/test/string_test.cpp \
    $$PWD/test/array_test.cpp \
    $$PWD/test/file_test.cpp \
    $$PWD/test/error_test.cpp \
    $$PWD/test/socket_test.cpp \
    $$PWD/ui/win32/window_win32.cpp \
    $$PWD/ui/win32/windowproc_win32.cpp \
    $$PWD/ui/win32/control_win32.cpp \
    $$PWD/ui/win32/button_win32.cpp \
    $$PWD/ui/win32/edit_win32.cpp \
    $$PWD/ui/win32/font_win32.cpp \
    $$PWD/ui/win32/table_win32.cpp \
    $$PWD/ui/win32/progress_win32.cpp \
    $$PWD/ui/win32/align_win32.cpp

OTHER_FILES += \
    $$PWD/CMakeLists.txt \
    $$PWD/ui/CMakeLists.txt \
    $$PWD/bbb/CMakeLists.txt

INCLUDEPATH += \
    $$PWD \
    $$PWD/lib \
    $$PWD/test \
    $$PWD/ui \
    $$PWD/bbb

# Build
#DEFINES += ZLOG_STD_MUTEX
QMAKE_CXXFLAGS += -Wall -Wextra -pedantic -ansi -std=c++11
LIBS += pthread

win32:DEFINES += PLATFORM_WINDOWS COMPILER_MINGW
else:DEFINES += PLATFORM_LINUX COMPILER_GCC

#include($$PWD/ui/LibChaosUI.pri)
