## LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(LibChaos)

SET(LibChaos_HEADERS ### =================== HEADERS =================== ###
    database.h
    zclock.h
    zerror.h
    zfile.h
    zlog.h
    zlogworker.h
    zmutex.h
    zautobuffer.h
    zautobufferunit.h
    zret.h
    zthread.h
    ztime.h
    ztypes.h

    data/zarray.h
    data/zarray2.h
    data/zassoc.h
    data/zassoc2.h
    data/zbinary.h
    data/zdata.h
    data/zmap.h
    data/zpath.h
    data/zjson.h
    data/zstring.h
    data/zxml.h

    net/zaddress.h
    net/zconnection.h
    net/zsocket.h
    net/zdatagramsocket.h
    net/zstreamsocket.h
    net/zstreamconnection.h

    lib/sqlite3.h
    #lib/demangle.h

    net/zaddress.h
    net/zconnection.h
    net/zsocket.h
    net/zdatagramsocket.h
    net/zstreamsocket.h
    net/zstreamconnection.h
)

SET(LibChaos_SOURCES ### =================== SOURCES =================== ###
    database.cpp
    zautobuffer.cpp
    #zclock.cpp
    zerror.cpp
    zfile.cpp
    zlog.cpp
    zlogworker.cpp

    zthread.cpp
    #ztime.cpp

    data/zbinary.cpp
    data/zjson.cpp
    data/zpath.cpp
    data/zstring.cpp
    data/zstring_encode.cpp
    data/zxml.cpp

    net/zaddress.cpp
    net/zconnection.cpp
    net/zsocket.cpp
    net/zdatagramsocket.cpp
    net/zstreamsocket.cpp
    net/zstreamconnection.cpp

    lib/sqlite3.c

    net/zaddress.cpp
    net/zconnection.cpp
    net/zsocket.cpp
    net/zdatagramsocket.cpp
    net/zstreamsocket.cpp
    net/zstreamconnection.cpp
)

IF(LIBCHAOS_WINDOWS)
    SET(LibChaos_SOURCES
        ${LibChaos_SOURCES}
        #lib/StackWalker.cpp
    )
ENDIF()

### =================== CONFIG =================== ###

SET(LibChaos_INCLUDE ${LibChaos_SOURCE_DIR} ${LibChaos_SOURCE_DIR}/net ${LibChaos_SOURCE_DIR}/data PARENT_SCOPE)

INCLUDE_DIRECTORIES(
    ${LibChaos_SOURCE_DIR}
    net
    lib
    data
    ${CMAKE_SOURCE_DIR}
)

#source_hash(${LibChaos_SOURCE_DIR})
configure_build("Core" ${LIBCHAOS_BUILD} ${LIBCHAOS_PLATFORM})
SET(LibChaosCore_BUILD_STRING ${BUILD_STRING} PARENT_SCOPE)

ADD_DEFINITIONS(-DZLOG_USE_THREAD)
#ADD_DEFINITIONS(-DZLOG_STD_MUTEX)
IF(IBERTY_DEMANGLE)
    ADD_DEFINITIONS(-DIBERTY_DEMANGLE)
ENDIF()

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -ansi -std=c++11")
#SET_SOURCE_FILES_PROPERTIES(sqlite3.c PROPERTIES COMPILE_FLAGS -std=c99)

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

ADD_LIBRARY(chaos ${LibChaos_SOURCES})
TARGET_LINK_LIBRARIES(chaos pthread)
IF(LIBCHAOS_UNIX)
    IF(IBERTY_DEMANGLE)
        TARGET_LINK_LIBRARIES(chaos iberty)
    ENDIF()
    IF(FREEBSD)
        TARGET_LINK_LIBRARIES(chaos execinfo)
    ELSE()
        TARGET_LINK_LIBRARIES(chaos dl)
    ENDIF()
ELSEIF(LIBCHAOS_WINDOWS)
    TARGET_LINK_LIBRARIES(chaos ws2_32 dbghelp kernel32 psapi version)
ENDIF()
