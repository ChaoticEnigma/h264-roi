## LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(LibChaos_Extern)

OPTION(USE_ZLIB_LIBPNG "Link against and use zlib and libpng" ON)
OPTION(FORCE_INTERNAL_ZLIB_LIBPNG "Use internal version of zlib and libpng" OFF)

IF(LIBCHAOS_WINDOWS OR FORCE_INTERNAL_ZLIB_LIBPNG)
    SET(USE_INTERNAL_ZLIB_LIBPNG TRUE)
ENDIF()

IF(USE_ZLIB_LIBPNG AND USE_INTERNAL_ZLIB_LIBPNG)
    ADD_SUBDIRECTORY(lib/zlib-1.2.8)
    ADD_SUBDIRECTORY(lib/libpng-1.2.37)
ENDIF()

SET(LibChaos_Extern_HEADERS ### =================== HEADERS =================== ###
    zjpeg.h
    zpng.h
)

SET(LibChaos_Extern_SOURCES ### =================== SOURCES =================== ###
    zjpeg.cpp
    zpng.cpp
)

### =================== CONFIG =================== ###

# Export Include Path
SET(LibChaosExtern_INCLUDE
    ${LibChaos_Extern_SOURCE_DIR}
)
IF(USE_INTERNAL_ZLIB_LIBPNG)
    SET(LibChaosExtern_INCLUDE ${LibChaosExtern_INCLUDE} ${libpng_INCLUDE})
ENDIF()

SET(LibChaosExtern_INCLUDE ${LibChaosExtern_INCLUDE} PARENT_SCOPE)

# Include Directories
INCLUDE_DIRECTORIES(
    ${LibChaos_INCLUDE}
    lib
    ${libpng_INCLUDE}
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

IF(USE_INTERNAL_ZLIB_LIBPNG OR LIBCHAOS_WINDOWS)
    TARGET_LINK_LIBRARIES(chaosextern libpng zlib)
ELSE()
    TARGET_LINK_LIBRARIES(chaosextern png z)
ENDIF()
