## Main LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

PROJECT(LibChaos)

# Options
OPTION(LIBCHAOS_ZPARCEL "Build ZParcel tool"                        OFF )

OPTION(LIBCHAOS_EXTERN  "Build LibChaos with External Libraries"    OFF )
OPTION(LIBCHAOS_UI      "Build LibChaos UI library"                 OFF )
OPTION(LIBCHAOS_TEST    "Build LibChaos Test suite"                 ON  )
OPTION(LIBCHAOS_UTIL    "Build LibChaos Utilities"                  OFF )
OPTION(LIBCHAOS_BBB     "Build LibChaos BeagleBone Black library"   OFF )

# Build Settings
INCLUDE(cmake/ConfigureBuild.cmake)
#INCLUDE(cmake/source_hash.cmake)

# Export Options
SET(LibChaos_COMPILER   ${LIBCHAOS_COMPILER}    PARENT_SCOPE)
SET(LibChaos_PLATFORM   ${LIBCHAOS_PLATFORM}    PARENT_SCOPE)
SET(LibChaos_BUILD      ${LIBCHAOS_BUILD}       PARENT_SCOPE)

SET(COMPILER_GCC        ${COMPILER_GCC}         PARENT_SCOPE)
SET(COMPILER_MINGW      ${COMPILER_MINGW}       PARENT_SCOPE)
SET(COMPILER_MSVC       ${COMPILER_MSVC}        PARENT_SCOPE)
SET(COMPILER_CLANG      ${COMPILER_CLANG}       PARENT_SCOPE)

SET(PLATFORM_LINUX      ${PLATFORM_LINUX}       PARENT_SCOPE)
SET(PLATFORM_FREEBSD    ${PLATFORM_FREEBSD}     PARENT_SCOPE)
SET(PLATFORM_WINDOWS    ${PLATFORM_WINDOWS}     PARENT_SCOPE)
SET(PLATFORM_MACOSX     ${PLATFORM_MACOSX}      PARENT_SCOPE)

# Readme Files
ADD_CUSTOM_TARGET(dummy SOURCES
    doc/README.txt
    doc/Projects.txt
    doc/LibChaos_Todo_Ideas.txt
)

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

# Git Describe
EXECUTE_PROCESS(
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND git describe --all --long --always --abbrev=8 --dirty=*
    OUTPUT_VARIABLE LIBCHAOS_DESCRIBE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Doxygen
FIND_PACKAGE(Doxygen)
IF(DOXYGEN_FOUND)
    IF(LIBCHAOS_DOXYGEN)
        SET(DOXYGEN_AUTO ALL)
    ENDIF()

    # General Doxygen Options
    SET(DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR})
    SET(DOXYGEN_OUTPUT ${CMAKE_CURRENT_BINARY_DIR})
    SET(DOXYGEN_VERSION ${LIBCHAOS_DESCRIBE})
    IF(DEBUG)
        SET(DOXYGEN_DEBUG "YES")
    ELSE()
        SET(DOXYGEN_DEBUG "NO")
    ENDIF()

    # Build Compressed Help on Windows
    INCLUDE(cmake/FindHTMLHelp.cmake)
    IF(HTML_HELP_COMPILER)
        SET(DOXYGEN_HHC ${HTML_HELP_COMPILER})
        CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile-chm.in" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-chm" @ONLY)
        ADD_CUSTOM_TARGET(doxyhelp ${DOXYGEN_AUTO}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-chm"
            COMMAND ${CMAKE_COMMAND} -E rename chm/LibChaos.chm LibChaos.chm
        )
    ENDIF()

    # Build Normal HTML
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)
    ADD_CUSTOM_TARGET(doxygen ${DOXYGEN_AUTO}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    )
ENDIF()

# Build Status Line
MESSAGE(STATUS "LibChaos: ${CONFIGURE_BUILD_STRING} - ${LibChaos_BUILD_LIST} - ${LIBCHAOS_DESCRIBE}")
