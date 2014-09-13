## LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(LibChaos)

OPTION(IBERTY_DEMANGLE "Use libiberty to demangle symbols" OFF)
OPTION(FORCE_INTERNAL_ZLIB_LIBPNG "Use internal version of zlib and libpng" OFF)

IF(LIBCHAOS_WINDOWS OR FORCE_INTERNAL_ZLIB_LIBPNG)
    SET(USE_INTERNAL_ZLIB_LIBPNG TRUE)
ENDIF()

IF(USE_INTERNAL_ZLIB_LIBPNG)
    ADD_SUBDIRECTORY(lib/zlib-1.2.8)
    ADD_SUBDIRECTORY(lib/libpng-1.2.37)
ENDIF()

SET(LibChaos_HEADERS ### =================== HEADERS =================== ###
    zclock.h
    zerror.h
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

    file/database.h
    file/zbitmap.h
    file/zfile.h
    file/zpng.h

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
    zautobuffer.cpp
    #zclock.cpp
    zerror.cpp
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

    file/database.cpp
    file/zbmp.cpp
    file/zfile.cpp
    file/zpng.cpp

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

# Export Include Path
SET(LibChaos_INCLUDE
    ${LibChaos_SOURCE_DIR}
    ${LibChaos_SOURCE_DIR}/data
    ${LibChaos_SOURCE_DIR}/file
    ${LibChaos_SOURCE_DIR}/net
)
IF(USE_INTERNAL_ZLIB_LIBPNG)
    SET(LibChaos_INCLUDE ${LibChaos_INCLUDE} ${libpng_INCLUDE})
ENDIF()

SET(LibChaos_INCLUDE ${LibChaos_INCLUDE} PARENT_SCOPE)

# Include Directories
INCLUDE_DIRECTORIES(
    ${LibChaos_SOURCE_DIR}
    data
    file
    lib
    net
    ${libpng_INCLUDE}
)
IF(USE_INTERNAL_ZLIB_LIBPNG)
    INCLUDE_DIRECTORIES(
        ${libpng_INCLUDE}
    )
ENDIF()

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
    IF(USE_INTERNAL_ZLIB_LIBPNG)
        TARGET_LINK_LIBRARIES(chaos libpng zlib)
    ELSE()
        TARGET_LINK_LIBRARIES(chaos png z)
    ENDIF()

    IF(IBERTY_DEMANGLE)
        TARGET_LINK_LIBRARIES(chaos iberty)
    ENDIF()

    IF(FREEBSD)
        TARGET_LINK_LIBRARIES(chaos execinfo)
    ELSE()
        TARGET_LINK_LIBRARIES(chaos dl)
    ENDIF()
ELSEIF(LIBCHAOS_WINDOWS)
    TARGET_LINK_LIBRARIES(chaos ws2_32 dbghelp kernel32 psapi version libpng zlib)
ENDIF()
