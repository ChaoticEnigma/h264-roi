TARGET = chaos
TEMPLATE = lib
CONFIG += staticlib console
QT -= core gui widgets

HEADERS += \
    $$PWD/zstring.h \
    $$PWD/zlog.h \
    $$PWD/zfile.h \
    $$PWD/database.h \
    $$PWD/zmutex.h \
    $$PWD/zpath.h \
    $$PWD/zthread.h \
    $$PWD/zlogworker.h \
    $$PWD/zarray.h \
    $$PWD/zassoc.h \
    $$PWD/zjson.h \
    $$PWD/ztypes.h \
    $$PWD/zbinary.h \
    $$PWD/zarray2.h \
    $$PWD/zassoc2.h \
    $$PWD/zautobuffer.h \
    $$PWD/zautobufferunit.h \
    $$PWD/zerror.h \
    $$PWD/zmap.h \
    $$PWD/lib/sqlite3.h \
    $$PWD/test/test.h

SOURCES += \
    $$PWD/zstring.cpp \
    $$PWD/zlog.cpp \
    $$PWD/zfile.cpp \
    $$PWD/database.cpp \
    $$PWD/zpath.cpp \
    $$PWD/zthread.cpp \
    $$PWD/zlogworker.cpp \
    $$PWD/zjson.cpp \
    $$PWD/zbinary.cpp \
    $$PWD/zstring_encode.cpp \
    $$PWD/zautobuffer.cpp \
    $$PWD/zerror.cpp \
    $$PWD/lib/sqlite3.c \
    $$PWD/test/libchaos-main.cpp \
    $$PWD/test/autobuffer_test.cpp \
    $$PWD/test/thread_test.cpp \
    $$PWD/test/string_test.cpp \
    $$PWD/test/array_test.cpp \
    $$PWD/test/file_test.cpp \
    $$PWD/test/error_test.cpp

OTHER_FILES += $$PWD/CMakeLists.txt

INCLUDEPATH += $$PWD \
    $$PWD/test \
    $$PWD/lib

#DEFINES += ZLOG_STD_MUTEX
QMAKE_CXXFLAGS += -Wall -Wextra -pedantic -ansi -std=c++11
LIBS += pthread

win32:DEFINES += PLATFORM_WINDOWS COMPILER_MINGW
else:DEFINES += PLATFORM_LINUX COMPILER_GCC

include($$PWD/ui/LibChaosUI.pri)
