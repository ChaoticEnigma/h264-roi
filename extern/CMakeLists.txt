## LibChaos Extern CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

PROJECT(LibChaos_Extern)

# Find libpng and zlib
INCLUDE(FindPNG)

IF(PNG_FOUND AND ENABLE_ZPNG)
    SET(BUILD_PNG TRUE)
    SET(LibChaos_Has_PNG TRUE PARENT_SCOPE)
ELSE()
    SET(BUILD_PNG FALSE)
    SET(LibChaos_Has_PNG FALSE PARENT_SCOPE)
ENDIF()

# Enable all extern classes
IF(ENABLE_ALL_EXTERN)
    SET(ENABLE_ZPNG 1)
    SET(ENABLE_ZPNG 1 PARENT_SCOPE)
    SET(ENABLE_ZJPEG 1)
    SET(ENABLE_ZJPEG 1 PARENT_SCOPE)
    SET(ENABLE_ZH264 1)
    SET(ENABLE_ZH264 1 PARENT_SCOPE)
ENDIF()

### =================== HEADERS =================== ###
SET(LibChaos_Extern_HEADERS
    zjpeg.h
    zh264decoder.h
    zh264encoder.h
)

### =================== SOURCES =================== ###
IF(BUILD_PNG)
    SET(EXTERN_BUILD_LIST ${EXTERN_BUILD_LIST} "PNG")
    SET(LibChaos_Extern_SOURCES
        zpng.h
        zpng.cpp
    )
ENDIF()
IF(ENABLE_ZJPEG)
    SET(EXTERN_BUILD_LIST ${EXTERN_BUILD_LIST} "JPEG")
    SET(LibChaos_Extern_SOURCES ${LibChaos_Extern_SOURCES} zjpeg.cpp)
ENDIF()
IF(ENABLE_ZH264)
    SET(EXTERN_BUILD_LIST ${EXTERN_BUILD_LIST} "H264")
    SET(LibChaos_Extern_SOURCES ${LibChaos_Extern_SOURCES}
        zh264decoder.cpp
        zh264encoder.cpp
    )
ENDIF()

SET(LibChaos_Extern_FILES ${LibChaos_Extern_HEADERS} ${LibChaos_Extern_SOURCES} PARENT_SCOPE)

### =================== CONFIG =================== ###

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

IF(BUILD_PNG)
    ADD_DEFINITIONS(${PNG_DEFINITIONS} -DLIBCHAOS_HAS_PNG)
ENDIF()

### =================== BUILD =================== ###

ADD_LIBRARY(chaosextern ${LIBCHAOS_EXTERN_ALL} ${LibChaos_Extern_HEADERS} ${LibChaos_Extern_SOURCES})
TARGET_LINK_LIBRARIES(chaosextern chaos)

# Link libpng and zlib
IF(BUILD_PNG)
    TARGET_LINK_LIBRARIES(chaosextern ${PNG_LIBRARIES})
ENDIF()
# Link libjpeg
IF(ENABLE_ZJPEG)
    TARGET_LINK_LIBRARIES(chaosextern jpeg)
ENDIF()
# Link x264 and libavcodec
IF(ENABLE_ZH264)
    TARGET_LINK_LIBRARIES(chaosextern avcodec avutil swscale x264)
ENDIF()
