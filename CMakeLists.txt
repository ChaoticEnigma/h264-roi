## H264-ROI CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(H264_ROI)

ADD_SUBDIRECTORY(libchaos)

### =================== SOURCES =================== ###

SET(H264_ROI_SOURCES
    main.h
    main.cpp

    zh264decoder.h
    zh264decoder.cpp
    zh264encoder.h
    zh264encoder.cpp
)

### =================== CONFIG =================== ###

INCLUDE_DIRECTORIES(
    ${H264_ROI_SOURCE_DIR}
    ${LibChaos_INCLUDE}
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LibChaos_CXX_FLAGS}")

CONFIGURE_FILE(roi.sh roi.sh COPYONLY)

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

# H264-ROI
ADD_EXECUTABLE(h264_roi ${H264_ROI_SOURCES})
LibChaos_Configure_Target(h264_roi)

IF(LibChaos_PLATFORM_WINDOWS)
    TARGET_LINK_LIBRARIES(h264_roi avformat avcodec avdevice avutil swscale swresample fdk-aac x264)
    #TARGET_LINK_LIBRARIES(h264_roi C:/bin/TDMGCC/MinGW64/lib/libfdk-aac.a)
    #TARGET_LINK_LIBRARIES(h264_roi C:/bin/TDMGCC/MinGW64/lib/libx264.a)
ELSE()
    TARGET_LINK_LIBRARIES(h264_roi avformat avcodec avdevice avutil swscale swresample x264 z)
    #TARGET_LINK_LIBRARIES(h264_roi avformat avcodec avdevice avutil swscale swresample x264 z mp3lame)
ENDIF()

ADD_CUSTOM_TARGET(h264-dummy SOURCES README)
