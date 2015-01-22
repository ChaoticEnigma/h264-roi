TEMPLATE = app
CONFIG += console
CONFIG -= qt

HEADERS += \
    $$PWD/zparcel.h \
    $$PWD/parcel-main.h

SOURCES += \
    $$PWD/parcel-main.cpp \
    $$PWD/zparcel.cpp \
    $$PWD/manual.cpp \
    $$PWD/auto.cpp

INCLUDEPATH += ../libchaos

OTHER_FILES += \
    $$PWD/CMakeLists.txt
