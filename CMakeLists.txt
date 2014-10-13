## H264-ROI CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(H264_ROI)

ADD_SUBDIRECTORY(libchaos)

### =================== HEADERS =================== ###

SET(H264_ROI_SOURCES ### =================== SOURCES =================== ###
    main.cpp
)

### =================== CONFIG =================== ###

INCLUDE_DIRECTORIES(
    ${H264_ROI_SOURCE_DIR}
    ${LibChaos_INCLUDE}
)

INCLUDE(${LibChaos_CONFIGURE_CMAKE})
configure_build("H264_ROI" ${LibChaos_BUILD} ${LibChaos_PLATFORM})
MESSAGE(STATUS ${BUILD_STRING})

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

# Extension Fixer
ADD_EXECUTABLE(h264_roi ${H264_ROI_SOURCES})
TARGET_LINK_LIBRARIES(h264_roi chaos)
