## LibChaos Extern CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

PROJECT(LibChaosExtern)

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

SET(LibChaosExtern_HEADERS
    zpng.h
    zjpeg.h
    zh264decoder.h
    zh264encoder.h
)

### =================== SOURCES =================== ###

SET(LibChaosExtern_PNG_SOURCES
    zpng.cpp
)

SET(LibChaosExtern_JPEG_SOURCES
    zjpeg.cpp
)

SET(LibChaosExtern_H264_SOURCES
    zh264decoder.cpp
    zh264encoder.cpp
)

### =================== CONFIG =================== ###

IF(WITH_PNG)
    SET(EXTERN_BUILD_LIST ${EXTERN_BUILD_LIST} "PNG")
    SET(LibChaosExtern_SOURCES ${LibChaosExtern_SOURCES} ${LibChaosExtern_PNG_SOURCES})
ENDIF()

IF(WITH_JPEG)
    SET(EXTERN_BUILD_LIST ${EXTERN_BUILD_LIST} "JPEG")
    SET(LibChaosExtern_SOURCES ${LibChaosExtern_SOURCES} ${LibChaosExtern_JPEG_SOURCES})
ENDIF()

IF(WITH_H264)
    SET(EXTERN_BUILD_LIST ${EXTERN_BUILD_LIST} "H264")
    SET(LibChaosExtern_SOURCES ${LibChaosExtern_SOURCES} ${LibChaosExtern_H264_SOURCES})
ENDIF()

# Export Build List
SET(EXTERN_BUILD_LIST ${EXTERN_BUILD_LIST} PARENT_SCOPE)

# Export Properties
GatherSources(${LibChaosExtern_HEADERS} ${LibChaosExtern_PNG_SOURCES} ${LibChaosExtern_JPEG_SOURCES} ${LibChaosExtern_H264_SOURCES})
GatherIncludes(.)

# Include Directories
GET_PROPERTY(LibChaosInclude GLOBAL PROPERTY LibChaosInclude)
INCLUDE_DIRECTORIES(
    ${LibChaosInclude}
    ${PNG_INCLUDE_DIRS}
)

IF(WITH_PNG)
    ADD_DEFINITIONS(${PNG_DEFINITIONS} -DLIBCHAOS_HAS_PNG)
ENDIF()

### =================== BUILD =================== ###

ADD_LIBRARY(chaosextern ${LIBCHAOS_EXTERN_ALL} ${LibChaosExtern_HEADERS} ${LibChaosExtern_SOURCES})

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
