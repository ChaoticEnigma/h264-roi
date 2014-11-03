## LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(LibChaos)

OPTION(IBERTY_DEMANGLE "Use libiberty to demangle symbols" OFF)

SET(LibChaos_HEADERS ### =================== HEADERS =================== ###
    zclock.h
    zerror.h
    zautobuffer.h
    zautobufferunit.h
    zret.h
    ztime.h
    ztypes.h

    base/zlog.h
    base/zlogworker.h
    base/zpath.h
    base/zstring.h

    data/zarray.h
    data/zarray2.h
    data/zassoc.h
    data/zassoc2.h
    data/zbinary.h
    data/zdata.h
    data/zmap.h
    data/zjson.h
    data/zxml.h

    file/database.h
    file/zbitmap.h
    file/zbmp.h
    file/zfile.h
    file/zimage.h
    file/zppm.h

    net/zaddress.h
    net/zconnection.h
    net/zsocket.h
    net/zdatagramsocket.h
    net/zstreamsocket.h

    math/znumber.h

    lib/sqlite3.h
    #lib/demangle.h

    thread/zmutex.h
    thread/zthread.h
)

SET(LibChaos_SOURCES ### =================== SOURCES =================== ###
    zautobuffer.cpp
    #zclock.cpp
    zerror.cpp
    #ztime.cpp

    base/zlog.cpp
    base/zlogworker.cpp
    base/zpath.cpp
    base/zstring.cpp
    base/zstring_encode.cpp

    data/zbinary.cpp
    data/zjson.cpp
    data/zxml.cpp

    file/database.cpp
    file/zbmp.cpp
    file/zfile.cpp
    file/zimage.cpp
    file/zppm.cpp

    lib/sqlite3.c
    lib/xxhash.c

    net/zaddress.cpp
    net/zconnection.cpp
    net/zsocket.cpp
    net/zdatagramsocket.cpp
    net/zstreamsocket.cpp

    math/znumber.cpp
    math/znumber-reference.cpp

    thread/zthread.cpp
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
    ${LibChaos_SOURCE_DIR}/abstract
    ${LibChaos_SOURCE_DIR}/base
    ${LibChaos_SOURCE_DIR}/data
    ${LibChaos_SOURCE_DIR}/file
    ${LibChaos_SOURCE_DIR}/net
    ${LibChaos_SOURCE_DIR}/math
    ${LibChaos_SOURCE_DIR}/thread
)

SET(LibChaos_INCLUDE ${LibChaos_INCLUDE} PARENT_SCOPE)

# Include Directories
INCLUDE_DIRECTORIES(
    ${LibChaos_SOURCE_DIR}
    abstract
    base
    data
    file
    lib
    net
    math
    thread
)

#source_hash(${LibChaos_SOURCE_DIR})
configure_build("Core" ${LIBCHAOS_BUILD} ${LIBCHAOS_PLATFORM})
SET(LibChaosCore_BUILD_STRING ${BUILD_STRING} PARENT_SCOPE)

ADD_DEFINITIONS(-DZLOG_USE_THREAD)
#ADD_DEFINITIONS(-DZLOG_STD_MUTEX)

IF(IBERTY_DEMANGLE)
    ADD_DEFINITIONS(-DIBERTY_DEMANGLE)
ENDIF()

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
