## LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(LibChaos_Extern)

OPTION(USE_ZLIB_LIBPNG "Link against and use zlib and libpng" OFF)
OPTION(FORCE_INTERNAL_ZLIB_LIBPNG "Use internal version of zlib and libpng" OFF)

OPTION(ENABLE_ZWEBP "Link libwebp and build ZWebP" OFF)

OPTION(ENABLE_ZH264 "Link libavcodec and libx264 and build ZH264Decoder and ZH264Encoder" OFF)

IF(LIBCHAOS_WINDOWS OR FORCE_INTERNAL_ZLIB_LIBPNG)
    SET(USE_INTERNAL_ZLIB_LIBPNG TRUE)
ENDIF()

IF(USE_ZLIB_LIBPNG AND USE_INTERNAL_ZLIB_LIBPNG)
    ADD_SUBDIRECTORY(lib/zlib-1.2.8)
    ADD_SUBDIRECTORY(lib/libpng-1.2.37)
ENDIF()

SET(LibChaos_Extern_HEADERS ### =================== HEADERS =================== ###
    zh264decoder.h
    zh264encoder.h
    zjpeg.h
    zpng.h
)

SET(LibChaos_Extern_SOURCES ### =================== SOURCES =================== ###

)
IF(USE_ZLIB_LIBPNG)
    SET(LibChaos_Extern_SOURCES ${LibChaos_Extern_SOURCES}
        zjpeg.cpp
        zpng.cpp
    )
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
IF(USE_INTERNAL_ZLIB_LIBPNG)
    SET(LibChaosExtern_INCLUDE ${LibChaosExtern_INCLUDE} ${libpng_INCLUDE})
ENDIF()

# Export Include Path
SET(LibChaosExtern_INCLUDE ${LibChaosExtern_INCLUDE} PARENT_SCOPE)

# Include Directories
INCLUDE_DIRECTORIES(
    ${LibChaos_INCLUDE}
    lib
)
IF(USE_INTERNAL_ZLIB_LIBPNG)
    INCLUDE_DIRECTORIES(
        ${libpng_INCLUDE}
    )
ENDIF()

configure_build("Extern" ${LIBCHAOS_BUILD} ${LIBCHAOS_PLATFORM})
SET(LibChaosExtern_BUILD_STRING ${BUILD_STRING} PARENT_SCOPE)

IF(NOT USE_ZLIB_LIBPNG)
    ADD_DEFINITIONS(-DDISABLE_LIBPNG)
    SET(DISABLE_LIBPNG 1 PARENT_SCOPE)
ENDIF()

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

ADD_LIBRARY(chaosextern ${LibChaos_Extern_SOURCES})
TARGET_LINK_LIBRARIES(chaosextern chaos)

IF(USE_ZLIB_LIBPNG)
    IF(USE_INTERNAL_ZLIB_LIBPNG OR LIBCHAOS_WINDOWS)
        TARGET_LINK_LIBRARIES(chaosextern libpng zlib)
    ELSE()
        TARGET_LINK_LIBRARIES(chaosextern png z)
    ENDIF()
ENDIF()

# Link x264 and libavcodec
IF(ENABLE_ZH264)
    TARGET_LINK_LIBRARIES(chaosextern x264 avcodec avutil swscale)
ENDIF()
