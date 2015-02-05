TEMPLATE = app
CONFIG += console
CONFIG -= qt

HEADERS += \
    $$PWD/zparcel.h \
    $$PWD/parcel-main.h \
    $$PWD/zparcel-parser.h \
    $$PWD/zparcel4-parser.h

SOURCES += \
    $$PWD/parcel-main.cpp \
    $$PWD/zparcel.cpp \
    $$PWD/zparcel4-parser.cpp

INCLUDEPATH += ../libchaos

OTHER_FILES += \
    $$PWD/CMakeLists.txt
