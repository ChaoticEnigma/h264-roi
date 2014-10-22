## LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(LibChaos_Extern)

OPTION(ENABLE_ZPNG "Link zlib and libpng and build ZPNG" OFF)
OPTION(ENABLE_ZJPEG "Link libjepg-turbo and build ZJPEG" OFF )
#OPTION(ENABLE_ZWEBP "Link libwebp and build ZWebP" OFF)
OPTION(ENABLE_ZH264 "Link libavcodec and libx264 and build ZH264Decoder and ZH264Encoder" OFF)

SET(LibChaos_Extern_HEADERS ### =================== HEADERS =================== ###
    zh264decoder.h
    zh264encoder.h
    zjpeg.h
    zpng.h
)

SET(LibChaos_Extern_SOURCES ### =================== SOURCES =================== ###
)
IF(ENABLE_ZPNG)
    SET(LibChaos_Extern_SOURCES ${LibChaos_Extern_SOURCES} zpng.cpp)
ENDIF()
IF(ENABLE_ZJPEG)
    SET(LibChaos_Extern_SOURCES ${LibChaos_Extern_SOURCES} zjpeg.cpp)
ENDIF()
IF(ENABLE_ZH264)
    SET(LibChaos_Extern_SOURCES ${LibChaos_Extern_SOURCES}
        zh264decoder.cpp
        zh264encoder.cpp
    )
ENDIF()

### =================== CONFIG =================== ###

SET(LibChaosExtern_INCLUDE
    ${LibChaos_Extern_SOURCE_DIR}
)

# Export Include Path
SET(LibChaosExtern_INCLUDE ${LibChaosExtern_INCLUDE} PARENT_SCOPE)

# Include Directories
INCLUDE_DIRECTORIES(
    ${LibChaos_INCLUDE}
)

configure_build("Extern" ${LIBCHAOS_BUILD} ${LIBCHAOS_PLATFORM})
SET(LibChaosExtern_BUILD_STRING ${BUILD_STRING} PARENT_SCOPE)

IF(NOT ENABLE_ZPNG)
    ADD_DEFINITIONS(-DDISABLE_LIBPNG)
    SET(DISABLE_LIBPNG 1 PARENT_SCOPE)
ENDIF()

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

IF(LibChaos_Extern_SOURCES)
    ADD_LIBRARY(chaosextern ${LibChaos_Extern_SOURCES})
    TARGET_LINK_LIBRARIES(chaosextern chaos)

    # Link libpng and zlib
    IF(ENABLE_ZPNG)
        TARGET_LINK_LIBRARIES(chaosextern png z)
    ENDIF()
    # Link libjpeg
    IF(ENABLE_ZJPEG)
        TARGET_LINK_LIBRARIES(chaosextern jpeg)
    ENDIF()
    # Link x264 and libavcodec
    IF(ENABLE_ZH264)
        TARGET_LINK_LIBRARIES(chaosextern avcodec avutil swscale)
        IF(PLATFORM_WINDOWS)
            TARGET_LINK_LIBRARIES(chaosextern C:/bin/TDMGCC/MinGW64/lib/libx264.a)
        ELSE()
            TARGET_LINK_LIBRARIES(chaosextern x264)
        ENDIF()
    ENDIF()
ENDIF()
