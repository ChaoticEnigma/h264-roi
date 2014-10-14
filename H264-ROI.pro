TEMPLATE = app
CONFIG += console
CONFIG -= qt

HEADERS += \
    main.h \
    roxlu/X264Encoder.h \
    roxlu/X264EncoderThreaded.h \
    roxlu/YUV420P.h \
    roxlu/H264_Decoder.h \
    roxlu/Utils.h \
    roxlu/tinylib-2.h \
    roxlu/tinylib.h \
    roxlu/dirent-win.h

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
    $$PWD/../x264 \
    $$PWD/../libuv/include \
    $$PWD/../ffmpeg \
    $$OUT_PWD/glxw/include

include(libchaos/LibChaos.pri)
