## H264-ROI CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(H264_ROI)

SET(LIBCHAOS_EXTERN 1)
SET(ENABLE_ZH264 1)
ADD_SUBDIRECTORY(libchaos)

### =================== HEADERS =================== ###

SET(H264_ROI_SOURCES ### =================== SOURCES =================== ###
    main.cpp

#    roxlu/H264_Decoder.cpp
#    roxlu/Utils.cpp
#    roxlu/X264Encoder.cpp
#    roxlu/X264EncoderThreaded.cpp
#    roxlu/YUV420P.cpp
)

### =================== CONFIG =================== ###

configure_build("H264_ROI" ${LibChaos_BUILD} ${LibChaos_PLATFORM})
MESSAGE(STATUS ${BUILD_STRING})

IF(PLATFORM_WINDOWS)
    SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -Wno-pedantic -Wno-sign-conversion -Wno-sign-compare -Wno-cast-qual")
    #SET_SOURCE_FILES_PROPERTIES(roxlu/Utils.cpp COMPILE_FLAGS " -Wno-pedantic")
ENDIF()

INCLUDE_DIRECTORIES(
    ${H264_ROI_SOURCE_DIR}
    ${LibChaos_INCLUDE}
)

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

# H264-ROI
ADD_EXECUTABLE(h264_roi ${H264_ROI_SOURCES})
TARGET_LINK_LIBRARIES(h264_roi chaos chaosextern)
IF(PLATFORM_WINDOWS)
    TARGET_LINK_LIBRARIES(h264_roi avformat avcodec avdevice avutil swscale swresample fdk-aac x264)
    #TARGET_LINK_LIBRARIES(h264_roi C:/bin/TDMGCC/MinGW64/lib/libfdk-aac.a)
    #TARGET_LINK_LIBRARIES(h264_roi C:/bin/TDMGCC/MinGW64/lib/libx264.a)
ELSE()
    TARGET_LINK_LIBRARIES(h264_roi avformat avcodec avdevice avutil swscale swresample x264 z)
ENDIF()
