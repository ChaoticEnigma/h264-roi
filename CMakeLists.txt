## LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(CMAKE_GENERATOR "Unix Makefiles")

PROJECT(LibChaos)

OPTION(LIBCHAOS_UI FALSE "Build LibChaos UI library")
OPTION(LIBCHAOS_BBB FALSE "Build LibChaos Beaglebone Black library")

IF(LIBCHAOS_UI)
    ADD_SUBDIRECTORY(ui)
    SET(LibChaosUI_INCLUDE ${LibChaosUI_INCLUDE} PARENT_SCOPE)
ENDIF()

IF(LIBCHAOS_BBB)
    ADD_SUBDIRECTORY(bbb)
    SET(LibChaosBBB_INCLUDE "${LibChaosBBB_SOURCE_DIR}" PARENT_SCOPE)
ENDIF()

SET(LibChaos_HEADERS ### =================== HEADERS =================== ###
    src/database.h
    src/zarray.h
    src/zarray2.h
    src/zassoc.h
    src/zassoc2.h
    src/zautobuffer.h
    src/zbinary.h
    src/zerror.h
    src/zfile.h
    src/zjson.h
    src/zlog.h
    src/zlogworker.h
    src/zmutex.h
    #zparcel.h
    src/zpath.h
    src/zstring.h
    src/zthread.h
    src/zxml.h
    src/zsocket.h

    lib/sqlite3.h
    lib/demangle.h
)

SET(LibChaos_SOURCES ### =================== SOURCES =================== ###
    src/database.cpp
    src/zautobuffer.cpp
    src/zbinary.cpp
    src/zerror.cpp
    src/zfile.cpp
    src/zjson.cpp
    src/zlog.cpp
    src/zlogworker.cpp
    #zparcel.cpp
    src/zpath.cpp
    src/zstring.cpp
    src/zstring_encode.cpp
    src/zthread.cpp
    src/zxml.cpp
    src/zsocket.cpp

    lib/sqlite3.c
)

SET(LibChaos_TEST_SOURCES ### =================== TEST_SOURCES =================== ###
    test/libchaos-main.cpp
    test/array_test.cpp
    test/string_test.cpp
    test/file_test.cpp
    test/thread_test.cpp
    test/autobuffer_test.cpp
    test/error_test.cpp
    test/socket_test.cpp
)

### =================== CONFIG =================== ###

SET(LibChaos_INCLUDE "${LibChaos_SOURCE_DIR}" PARENT_SCOPE)

INCLUDE_DIRECTORIES(
    ${LibChaos_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)

IF(WIN32)
    SET(WINDOWS TRUE)
ELSE()
    SET(UNIX TRUE)
ENDIF()

IF(DEBUG)
    ADD_DEFINITIONS(-DDEBUG)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    MESSAGE(STATUS "LibChaos Debug Build Starting...")
ELSEIF(RELEASE)
    ADD_DEFINITIONS(-DRELEASE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    MESSAGE(STATUS "LibChaos Release Build Starting...")
ELSE()
    MESSAGE(STATUS "LibChaos Normal Build Starting...")
ENDIF()

IF(WINDOWS)
    ADD_DEFINITIONS(-DPLATFORM_WINDOWS -DCOMPILER_MINGW)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwin32 -mwindows")
ELSE()
    ADD_DEFINITIONS(-DPLATFORM_LINUX -DCOMPILER_GCC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")
ENDIF()

ADD_DEFINITIONS(-DZLOG_USE_THREAD)
#ADD_DEFINITIONS(-DZLOG_STD_MUTEX)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -ansi -std=c++11")
#SET_SOURCE_FILES_PROPERTIES(sqlite3.c PROPERTIES COMPILE_FLAGS -std=c99)

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

ADD_LIBRARY(chaos ${LibChaos_SOURCES})
TARGET_LINK_LIBRARIES(chaos pthread)
IF(UNIX AND NOT FREEBSD)
    TARGET_LINK_LIBRARIES(chaos dl)
ENDIF()

IF(LIBCHAOS_TEST)
    ADD_EXECUTABLE(testchaos ${LibChaos_TEST_SOURCES})
    TARGET_LINK_LIBRARIES(testchaos chaos)
ENDIF()
