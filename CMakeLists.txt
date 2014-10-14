## H264-ROI CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(H264_ROI)

ADD_SUBDIRECTORY(libchaos)
ADD_SUBDIRECTORY(glxw)

#INCLUDE(roxlu/roxlu/build/cmake/Roxlu.cmake)

### =================== HEADERS =================== ###

SET(H264_ROI_SOURCES ### =================== SOURCES =================== ###
    main.cpp

    roxlu/Utils.cpp

    roxlu/X264Encoder.cpp
#    roxlu/X264EncoderThreaded.cpp
#    roxlu/YUV420P.cpp
)

### =================== CONFIG =================== ###

INCLUDE_DIRECTORIES(
    ${H264_ROI_SOURCE_DIR}
    ${LibChaos_INCLUDE}
    roxlu
    ${CMAKE_BINARY_DIR}/glxw/include
)

INCLUDE(${LibChaos_CONFIGURE_CMAKE})
configure_build("H264_ROI" ${LibChaos_BUILD} ${LibChaos_PLATFORM})
MESSAGE(STATUS ${BUILD_STRING})

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

# Extension Fixer
ADD_EXECUTABLE(h264_roi ${H264_ROI_SOURCES})
TARGET_LINK_LIBRARIES(h264_roi chaos uv x264 avcodec swscale avutil glxw)
