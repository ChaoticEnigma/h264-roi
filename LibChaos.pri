TARGET = chaos
TEMPLATE = lib
CONFIG += staticlib

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
    $$PWD/zassoc2.h

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
    $$PWD/zstring_encode.cpp

OTHER_FILES += $$PWD/CMakeLists.txt

INCLUDEPATH += $$PWD

#DEFINES += ZLOG_STD_MUTEX

win32:DEFINES += PLATFORM_WINDOWS COMPILER_MINGW
else:DEFINES += PLATFORM_LINUX COMPILER_GCC

include($$PWD/ui/LibChaosUI.pri)
