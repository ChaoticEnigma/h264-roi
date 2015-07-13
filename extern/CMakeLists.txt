## LibChaos Extern CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

PROJECT(LibChaos_Extern)

# Find libpng and zlib
INCLUDE(FindPNG)
IF(PNG_FOUND AND ENABLE_ZPNG)
    SET(WITH_PNG TRUE)
    SET(LibChaos_Has_PNG TRUE PARENT_SCOPE)
ELSE()
    SET(WITH_PNG FALSE)
    SET(LibChaos_Has_PNG FALSE PARENT_SCOPE)
ENDIF()

# Enable all extern classes
IF(ENABLE_ALL_EXTERN)
    SET(WITH_PNG 1)
    SET(WITH_PNG 1 PARENT_SCOPE)
    SET(WITH_JPEG 1)
    SET(WITH_JPEG 1 PARENT_SCOPE)
    SET(WITH_H264 1)
    SET(WITH_H264 1 PARENT_SCOPE)
ENDIF()

### =================== HEADERS =================== ###

SET(LibChaos_Extern_HEADERS
    zpng.h
    zjpeg.h
    zh264decoder.h
    zh264encoder.h
)

### =================== SOURCES =================== ###

SET(LibChaos_Extern_PNG_SOURCES
    zpng.cpp
)

SET(LibChaos_Extern_JPEG_SOURCES
    zjpeg.cpp
)

SET(LibChaos_Extern_H264_SOURCES
    zh264decoder.cpp
    zh264encoder.cpp
)

### =================== CONFIG =================== ###

IF(WITH_PNG)
    SET(EXTERN_BUILD_LIST ${EXTERN_BUILD_LIST} "PNG")
    SET(LibChaos_Extern_SOURCES ${LibChaos_Extern_SOURCES} ${LibChaos_Extern_PNG_SOURCES})
ENDIF()

IF(WITH_JPEG)
    SET(EXTERN_BUILD_LIST ${EXTERN_BUILD_LIST} "JPEG")
    SET(LibChaos_Extern_SOURCES ${LibChaos_Extern_SOURCES} ${LibChaos_Extern_JPEG_SOURCES})
ENDIF()

IF(WITH_H264)
    SET(EXTERN_BUILD_LIST ${EXTERN_BUILD_LIST} "H264")
    SET(LibChaos_Extern_SOURCES ${LibChaos_Extern_SOURCES} ${LibChaos_Extern_H264_SOURCES})
ENDIF()

GatherSources(${LibChaos_Extern_HEADERS} ${LibChaos_Extern_PNG_SOURCES} ${LibChaos_Extern_JPEG_SOURCES} ${LibChaos_Extern_H264_SOURCES})

SET(EXTERN_BUILD_LIST ${EXTERN_BUILD_LIST} PARENT_SCOPE)

SET(LibChaos_Extern_INCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Export Include Path
SET(LibChaos_Extern_INCLUDE ${LibChaos_Extern_INCLUDE} PARENT_SCOPE)

# Include Directories
INCLUDE_DIRECTORIES(
    ${LibChaos_INCLUDE}
    ${PNG_INCLUDE_DIRS}
)

IF(WITH_PNG)
    ADD_DEFINITIONS(${PNG_DEFINITIONS} -DLIBCHAOS_HAS_PNG)
ENDIF()

### =================== BUILD =================== ###

ADD_LIBRARY(chaosextern ${LIBCHAOS_EXTERN_ALL} ${LibChaos_Extern_HEADERS} ${LibChaos_Extern_SOURCES})

TARGET_LINK_LIBRARIES(chaosextern chaos)

# Link libpng and zlib
IF(WITH_PNG)
    TARGET_LINK_LIBRARIES(chaosextern ${PNG_LIBRARIES})
ENDIF()

# Link libjpeg
IF(WITH_JPEG)
    TARGET_LINK_LIBRARIES(chaosextern jpeg)
ENDIF()

# Link x264 and libav
IF(WITH_H264)
    TARGET_LINK_LIBRARIES(chaosextern avcodec avutil swscale x264)
ENDIF()
