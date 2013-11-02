TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += \
    libchaos/zstring.cpp \
    libchaos/zlog.cpp \
    libchaos/zfile.cpp \
    libchaos/sqlite3.c \
    libchaos/database.cpp \
    libchaos/zpath.cpp \
    libchaos/zthread.cpp \
    libchaos/libchaos-main.cpp \
    libchaos/zlogworker.cpp \
    libchaos/zjson.cpp \
    libchaos/zbinary.cpp

OTHER_FILES += \
    libchaos/CMakeLists.txt

HEADERS += \
    libchaos/zstring.h \
    libchaos/zlog.h \
    libchaos/zfile.h \
    libchaos/sqlite3.h \
    libchaos/database.h \
    libchaos/zmutex.h \
    libchaos/zpath.h \
    libchaos/zthread.h \
    libchaos/zlogworker.h \
    libchaos/zarray.h \
    libchaos/zassoc.h \
    libchaos/zjson.h \
    libchaos/ztypes.h \
    libchaos/zbinary.h \
    libchaos/zarray2.h \
    libchaos/zassoc2.h

INCLUDEPATH += libchaos
