## Main LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(LibChaos)

# Options
OPTION(LIBCHAOS_ZPARCEL "Build ZParcel tool"                        ON  )

OPTION(LIBCHAOS_EXTERN  "Build LibChaos with External Libraries"    OFF )
OPTION(LIBCHAOS_UI      "Build LibChaos UI library"                 OFF )
OPTION(LIBCHAOS_TEST    "Build LibChaos Test suite"                 ON  )
OPTION(LIBCHAOS_UTIL    "Build LibChaos Utilities"                  OFF )
OPTION(LIBCHAOS_BBB     "Build LibChaos BeagleBone Black library"   OFF )

# Build Settings
INCLUDE(cmake/configure_build.cmake)
#INCLUDE(cmake/source_hash.cmake)

# Platform Settings
IF(UNIX)
    SET(LIBCHAOS_UNIX TRUE)
ELSEIF(WIN32)
    SET(LIBCHAOS_WINDOWS TRUE)
ELSE()
    MESSAGE(ERROR "Unsupported Platform")
ENDIF()

SET(LibChaos_PLATFORM ${LIBCHAOS_PLATFORM} PARENT_SCOPE)

# Build Settings
IF(DEBUG)
    SET(LIBCHAOS_DEBUG TRUE)
    SET(LIBCHAOS_BUILD 1)
ELSEIF(RELEASE)
    SET(LIBCHAOS_RELEASE TRUE)
    SET(LIBCHAOS_BUILD 2)
ELSE()
    SET(LIBCHAOS_NORMAL TRUE)
    SET(LIBCHAOS_BUILD 3)
ENDIF()

SET(LibChaos_BUILD ${LIBCHAOS_BUILD} PARENT_SCOPE)

# Configure Build Options
configure_build(${LIBCHAOS_BUILD})

## Libraries

# LibChaos
ADD_SUBDIRECTORY(chaos)
SET(LibChaos_BUILD_LIST "Core")
SET(LibChaos_INCLUDE ${LibChaos_Core_INCLUDE})

# LibChaos Extern
IF(LIBCHAOS_EXTERN)
    ADD_SUBDIRECTORY(extern)
    SET(LibChaos_BUILD_LIST ${LibChaos_BUILD_LIST} "Extern")
    SET(LibChaos_INCLUDE ${LibChaos_INCLUDE} ${LibChaosExtern_INCLUDE})
ENDIF()

SET(LibChaos_INCLUDE ${LibChaos_INCLUDE} PARENT_SCOPE)

# LibChaos UI
IF(LIBCHAOS_UI)
    ADD_SUBDIRECTORY(ui)
    SET(LibChaos_BUILD_LIST ${LibChaos_BUILD_LIST} "UI")
    SET(LibChaosUI_INCLUDE ${LibChaosUI_INCLUDE} PARENT_SCOPE)
ENDIF()

# LibChaos BBB (Beaglebone Black)
IF(LIBCHAOS_BBB)
    ADD_SUBDIRECTORY(bbb)
    SET(LibChaos_BUILD_LIST ${LibChaos_BUILD_LIST} "BBB")
    SET(LibChaosBBB_INCLUDE ${LibChaosBBB_INCLUDE} PARENT_SCOPE)
ENDIF()

## Tools

# LibChaos Test
IF(LIBCHAOS_TEST)
    ADD_SUBDIRECTORY(test)
    SET(LibChaos_BUILD_LIST ${LibChaos_BUILD_LIST} "Test")
ENDIF()

# LibChaos Utils
IF(LIBCHAOS_UTIL)
    ADD_SUBDIRECTORY(util)
    SET(LibChaos_BUILD_LIST ${LibChaos_BUILD_LIST} "Util")
ENDIF()

# ZParcel
IF(LIBCHAOS_ZPARCEL)
    ADD_SUBDIRECTORY(zparcel)
    SET(LibChaos_BUILD_LIST ${LibChaos_BUILD_LIST} "ZParcel")
ENDIF()

MESSAGE(STATUS "LibChaos: ${CONFIGURE_BUILD_STRING} = ${LibChaos_BUILD_LIST}")
