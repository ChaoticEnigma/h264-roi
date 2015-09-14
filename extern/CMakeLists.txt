## LibChaos Extern CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(LibChaosExtern)

SET(WITH_PNG  FALSE)
SET(WITH_JPEG FALSE)
SET(WITH_H264 FALSE)

# Find libpng and zlib
INCLUDE(FindPNG)
IF(PNG_FOUND AND NOT DISABLE_ZPNG)
    SET(WITH_PNG TRUE)
ENDIF()

# Find libjpeg or libjpeg-turbo
INCLUDE(FindJPEG)
IF(JPEG_FOUND AND NOT DISABLE_ZJPEG)
    SET(WITH_JPEG TRUE)
ENDIF()

SET(LIBCHAOS_WITH_PNG  ${WITH_PNG}  PARENT_SCOPE)
SET(LIBCHAOS_WITH_JPEG ${WITH_JPEG} PARENT_SCOPE)
SET(LIBCHAOS_WITH_H264 ${WITH_H264} PARENT_SCOPE)

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
)

IF(WITH_PNG)
    INCLUDE_DIRECTORIES(
        ${PNG_INCLUDE_DIRS}
    )
    ADD_DEFINITIONS(${PNG_DEFINITIONS} -DLIBCHAOS_HAS_PNG)
ENDIF()

IF(WITH_JPEG)
    INCLUDE_DIRECTORIES(
        ${JPEG_INCLUDE_DIR}
    )
ENDIF()

### =================== BUILD =================== ###

ADD_LIBRARY(chaosextern ${LIBCHAOS_EXTERN_ALL} ${LibChaosExtern_HEADERS} ${LibChaosExtern_SOURCES})

TARGET_LINK_LIBRARIES(chaosextern chaos)

IF(WITH_PNG)
    TARGET_LINK_LIBRARIES(chaosextern ${PNG_LIBRARIES})
ENDIF()

IF(WITH_JPEG)
    TARGET_LINK_LIBRARIES(chaosextern ${JPEG_LIBRARIES})
ENDIF()

# Link x264 and libav
IF(WITH_H264)
    TARGET_LINK_LIBRARIES(chaosextern avcodec avutil swscale x264)
ENDIF()
