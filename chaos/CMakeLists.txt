## LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(LibChaos)

SET(LibChaos_HEADERS ### =================== HEADERS =================== ###
    database.h
    zaddress.h
    zarray.h
    zarray2.h
    zassoc.h
    zassoc2.h
    zautobuffer.h
    zbinary.h
    zconnection.h
    zerror.h
    zfile.h
    zjson.h
    zlog.h
    zlogworker.h
    zmutex.h
    #zparcel.h
    zpath.h
    zsocket.h
    zstring.h
    zthread.h
    zxml.h

    lib/sqlite3.h
    #lib/demangle.h
)

SET(LibChaos_SOURCES ### =================== SOURCES =================== ###
    database.cpp
    zaddress.cpp
    zautobuffer.cpp
    zbinary.cpp
    zconnection.cpp
    zerror.cpp
    zfile.cpp
    zjson.cpp
    zlog.cpp
    zlogworker.cpp
    #zparcel.cpp
    zpath.cpp
    zsocket.cpp
    zstring.cpp
    zstring_encode.cpp
    zthread.cpp
    zxml.cpp

    lib/sqlite3.c
)

### =================== CONFIG =================== ###

SET(LibChaos_INCLUDE "${LibChaos_SOURCE_DIR}" PARENT_SCOPE)

INCLUDE_DIRECTORIES(
    ${LibChaos_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)

INCLUDE(../configure_build.cmake)
configure_build("Core" ${LIBCHAOS_BUILD} ${LIBCHAOS_PLATFORM})
SET(LibChaosCore_BUILD_STRING ${BUILD_STRING} PARENT_SCOPE)

ADD_DEFINITIONS(-DZLOG_USE_THREAD)
#ADD_DEFINITIONS(-DZLOG_STD_MUTEX)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -ansi -std=c++11")
#SET_SOURCE_FILES_PROPERTIES(sqlite3.c PROPERTIES COMPILE_FLAGS -std=c99)

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

ADD_LIBRARY(chaos ${LibChaos_SOURCES})
TARGET_LINK_LIBRARIES(chaos pthread)
IF(LIBCHAOS_UNIX)
    IF(NOT FREEBSD)
        TARGET_LINK_LIBRARIES(chaos dl)
    ENDIF()
ELSEIF(LIBCHAOS_WINDOWS)
    TARGET_LINK_LIBRARIES(chaos ws2_32)
ENDIF()
