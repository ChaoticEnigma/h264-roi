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

INCLUDE(${LibChaos_CONFIGURE_CMAKE})
configure_build("H264_ROI" ${LibChaos_BUILD} ${LibChaos_PLATFORM})
MESSAGE(STATUS ${BUILD_STRING})

INCLUDE_DIRECTORIES(
    ${H264_ROI_SOURCE_DIR}
    ${LibChaos_INCLUDE}
    roxlu
    ${CMAKE_BINARY_DIR}/glxw/include
)
IF(PLATFORM_WINDOWS)
    INCLUDE_DIRECTORIES(
        ${CMAKE_SOURCE_DIR}/../x264
        ${CMAKE_SOURCE_DIR}/../libuv/include
        ${CMAKE_SOURCE_DIR}/../ffmpeg
    )
ENDIF()

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

# Extension Fixer
ADD_EXECUTABLE(h264_roi ${H264_ROI_SOURCES})
TARGET_LINK_LIBRARIES(h264_roi chaos)
IF(PLATFORM_WINDOWS)
    TARGET_LINK_LIBRARIES(h264_roi ${CMAKE_SOURCE_DIR}/../libuv/libuv.a)
    TARGET_LINK_LIBRARIES(h264_roi iphlpapi)

    TARGET_LINK_LIBRARIES(h264_roi ${CMAKE_SOURCE_DIR}/../x264/libx264.a)
    TARGET_LINK_LIBRARIES(h264_roi ${CMAKE_SOURCE_DIR}/../ffmpeg/libavcodec/libavcodec.a)
    TARGET_LINK_LIBRARIES(h264_roi ${CMAKE_SOURCE_DIR}/../ffmpeg/libswscale/libswscale.a)
    TARGET_LINK_LIBRARIES(h264_roi ${CMAKE_SOURCE_DIR}/../ffmpeg/libavutil/libavutil.a)
ELSE()
    TARGET_LINK_LIBRARIES(h264_roi uv x264 avcodec swscale avutil)
ENDIF()
TARGET_LINK_LIBRARIES(h264_roi glxw)
