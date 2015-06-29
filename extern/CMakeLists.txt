## LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

PROJECT(LibChaos_Extern)

INCLUDE(FindPNG)

OPTION(ENABLE_ZPNG "Link zlib and libpng and build ZPNG" OFF)
OPTION(ENABLE_ZJPEG "Link libjepg-turbo and build ZJPEG" OFF)
#OPTION(ENABLE_ZWEBP "Link libwebp and build ZWebP" OFF)
OPTION(ENABLE_ZH264 "Link libavcodec and libx264 and build ZH264Decoder and ZH264Encoder" OFF)

IF(PNG_FOUND AND ENABLE_ZPNG)
    SET(BUILD_PNG TRUE)
    SET(LIBCHAOS_HAS_PNG TRUE PARENT_SCOPE)
ELSE()
    SET(BUILD_PNG FALSE)
    SET(LIBCHAOS_HAS_PNG FALSE PARENT_SCOPE)
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
    LIST(APPEND LIBCHAOS_EXTERN_LIBS "PNG")
    SET(LibChaos_Extern_SOURCES
        zpng.h
        zpng.cpp
    )
ENDIF()
IF(ENABLE_ZJPEG)
    LIST(APPEND LIBCHAOS_EXTERN_LIBS "JPEG")
    SET(LibChaos_Extern_SOURCES ${LibChaos_Extern_SOURCES} zjpeg.cpp)
ENDIF()
IF(ENABLE_ZH264)
    LIST(APPEND LIBCHAOS_EXTERN_LIBS "H264")
    SET(LibChaos_Extern_SOURCES ${LibChaos_Extern_SOURCES}
        zh264decoder.cpp
        zh264encoder.cpp
    )
ENDIF()

SET(LibChaos_Extern_FILES ${LibChaos_Extern_HEADERS} ${LibChaos_Extern_SOURCES} PARENT_SCOPE)

### =================== CONFIG =================== ###

MESSAGE(STATUS "Extern Libs: " ${LIBCHAOS_EXTERN_LIBS})

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
