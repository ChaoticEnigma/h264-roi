TARGET = chaos
TEMPLATE = lib
CONFIG += staticlib console
QT -= core gui widgets

# LibChaos
HEADERS += \
    $$PWD/chaos/database.h \
    $$PWD/chaos/zarray.h \
    $$PWD/chaos/zarray2.h \
    $$PWD/chaos/zassoc.h \
    $$PWD/chaos/zassoc2.h \
    $$PWD/chaos/zautobuffer.h \
    $$PWD/chaos/zautobufferunit.h \
    $$PWD/chaos/zbinary.h \
    $$PWD/chaos/zerror.h \
    $$PWD/chaos/zfile.h \
    $$PWD/chaos/zjson.h \
    $$PWD/chaos/zlog.h \
    $$PWD/chaos/zlogworker.h \
    $$PWD/chaos/zmap.h \
    $$PWD/chaos/zmutex.h \
    $$PWD/chaos/zpath.h \
    $$PWD/chaos/zsocket.h \
    $$PWD/chaos/zstring.h \
    $$PWD/chaos/zthread.h \
    $$PWD/chaos/ztypes.h \
    $$PWD/chaos/zxml.h \
    $$PWD/chaos/lib/sqlite3.h \
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
    $$PWD/bbb/zbbb.h \
    chaos/zaddress.h \
    chaos/zconnection.h

SOURCES += \
    $$PWD/chaos/database.cpp \
    $$PWD/chaos/zautobuffer.cpp \
    $$PWD/chaos/zbinary.cpp \
    $$PWD/chaos/zerror.cpp \
    $$PWD/chaos/zfile.cpp \
    $$PWD/chaos/zjson.cpp \
    $$PWD/chaos/zlog.cpp \
    $$PWD/chaos/zlogworker.cpp \
    $$PWD/chaos/zpath.cpp \
    $$PWD/chaos/zsocket.cpp \
    $$PWD/chaos/zstring.cpp \
    $$PWD/chaos/zstring_encode.cpp \
    $$PWD/chaos/zthread.cpp \
    $$PWD/chaos/zxml.cpp \
    $$PWD/chaos/lib/sqlite3.c \
    $$PWD/test/libchaos_main.cpp \
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
    $$PWD/ui/win32/align_win32.cpp \
    $$PWD/bbb/io.cpp \
    test/socket2_test.cpp \
    chaos/zaddress.cpp \
    chaos/zconnection.cpp

OTHER_FILES += \
    $$PWD/CMakeLists.txt \
    $$PWD/ui/CMakeLists.txt \
    $$PWD/bbb/CMakeLists.txt \
    $$PWD/chaos/CMakeLists.txt \
    $$PWD/test/CMakeLists.txt \
    $$PWD/configure_build.cmake \
    $$PWD/todo.txt

INCLUDEPATH += \
    $$PWD \
    $$PWD/chaos \
    $$PWD/chaos/lib \
    $$PWD/test \
    $$PWD/ui \
    $$PWD/bbb

# Build
#DEFINES += ZLOG_STD_MUTEX
QMAKE_CXXFLAGS += -Wall -Wextra -pedantic -ansi -std=c++11
LIBS += pthread

win32:DEFINES += _LIBCHAOS_PLATFORM_WINDOWS _LIBCHAOS_COMPILER_MINGW
else:DEFINES += _LIBCHAOS_PLATFORM_LINUX _LIBCHAOS_COMPILER_GCC
