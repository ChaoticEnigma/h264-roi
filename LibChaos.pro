TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp \
    zarray.tpp \
    zstring.cpp

OTHER_FILES += \
    CMakeLists.txt

HEADERS += \
    zarray.h \
    zstring.h

