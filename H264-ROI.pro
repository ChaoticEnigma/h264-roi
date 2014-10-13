TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp

include(libchaos/LibChaos.pri)

OTHER_FILES += \
    CMakeLists.txt
