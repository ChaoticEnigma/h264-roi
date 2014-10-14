TEMPLATE = app
CONFIG += console
CONFIG -= qt

HEADERS += \
    roxlu/X264Encoder.h \
    roxlu/X264EncoderThreaded.h \
    roxlu/YUV420P.h \
    roxlu/main.h \
    roxlu/H264_Decoder.h \
    roxlu/tinylib.h \
    roxlu/Utils.h

SOURCES += \
    main.cpp \
    roxlu/X264Encoder.cpp \
    roxlu/X264EncoderThreaded.cpp \
    roxlu/YUV420P.cpp \
    roxlu/H264_Decoder.cpp \
    roxlu/Utils.cpp

OTHER_FILES += \
    CMakeLists.txt

INCLUDEPATH += \
    roxlu \
    $$OUT_PWD/glxw/include

include(libchaos/LibChaos.pri)
