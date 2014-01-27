TARGET = chaos
TEMPLATE = lib
CONFIG += staticlib console
QT -= core gui widgets

HEADERS += \
    $$PWD/zstring.h \
    $$PWD/zlog.h \
    $$PWD/zfile.h \
    $$PWD/sqlite3.h \
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
    libchaos/zautobuffer.h \
    libchaos/test/test.h \
    libchaos/zautobufferunit.h \
    libchaos/zerror.h \
    libchaos/zmap.h

SOURCES += \
    $$PWD/zstring.cpp \
    $$PWD/zlog.cpp \
    $$PWD/zfile.cpp \
    $$PWD/sqlite3.c \
    $$PWD/database.cpp \
    $$PWD/zpath.cpp \
    $$PWD/zthread.cpp \
    $$PWD/libchaos-main.cpp \
    $$PWD/zlogworker.cpp \
    $$PWD/zjson.cpp \
    $$PWD/zbinary.cpp \
    $$PWD/zstring_encode.cpp \
    libchaos/zautobuffer.cpp \
    libchaos/test/libchaos-main.cpp \
    libchaos/test/autobuffer_test.cpp \
    libchaos/test/thread_test.cpp \
    libchaos/test/string_test.cpp \
    libchaos/test/array_test.cpp \
    libchaos/test/file_test.cpp \
    libchaos/zerror.cpp \
    libchaos/test/error_test.cpp

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
