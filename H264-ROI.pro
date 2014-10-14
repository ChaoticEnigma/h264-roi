TEMPLATE = app
CONFIG += console
CONFIG -= qt

HEADERS += \
    roxlu/X264Encoder.h \
    roxlu/X264EncoderThreaded.h \
    roxlu/YUV420P.h \
    roxlu/main.h

SOURCES += \
    main.cpp \
    roxlu/X264Encoder.cpp \
    roxlu/X264EncoderThreaded.cpp \
    roxlu/YUV420P.cpp

OTHER_FILES += \
    CMakeLists.txt

INCLUDEPATH += \
    roxlu

include(libchaos/LibChaos.pri)
