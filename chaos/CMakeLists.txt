## LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(LibChaos_Core)

OPTION(IBERTY_DEMANGLE "Use libiberty to demangle symbols" OFF)

SET(LibChaos_Core_HEADERS ### =================== HEADERS =================== ###
    ztypes.h

    abstract/yindexedaccess.h
    abstract/yiterable.h
    abstract/ypushpopaccess.h
    abstract/zaccessor.h
    abstract/zbase.h
    abstract/zhashable.h
    abstract/ziterator.h
    abstract/zposition.h
    abstract/zreader.h
    abstract/zserializer.h
    abstract/zstorage.h
    abstract/zwriter.h

    base/zclock.h
    base/zerror.h
    base/zexception.h
    base/zlog.h
    base/zlogworker.h
    base/zpath.h
    base/zstring.h
    base/ztime.h

    data/zallocator.h
    data/zarray.h
    data/zassoc.h
    data/zbinary.h
    data/zdata.h
    data/zdefaultstorage.h
    data/zfilestorage.h
    data/zjson.h
    data/zlist.h
    data/zmap.h
    data/zpointer.h
    data/zqueue.h
    data/zstack.h
    data/zxml.h

    file/database.h
    file/zbitmap.h
    file/zbmp.h
    file/zfile.h
    file/zimage.h
    file/zppm.h

    lib/sqlite3.h
    #lib/demangle.h

    math/zexpression.h
    math/zformula.h
    math/znumber.h

    misc/zautobuffer.h
    misc/zautobufferunit.h
    misc/zencrypt.h
    misc/zhash.h
    misc/zmisc.h
    misc/zrandom.h
    misc/zret.h
    misc/zuid.h

    net/zaddress.h
    net/zconnection.h
    net/zdatagramsocket.h
    net/zsocket.h
    net/zstreamsocket.h

    thread/zcondition.h
    thread/zlock.h
    thread/zmutex.h
    thread/zthread.h
)

SET(LibChaos_Core_SOURCES ### =================== SOURCES =================== ###
    ${LibChaos_Core_HEADERS}

    base/zclock.cpp
    base/zerror.cpp
    base/zexception.cpp
    base/zlog.cpp
    base/zlogworker.cpp
    base/zpath.cpp
    base/zstring.cpp
    base/zstring_encode.cpp
    base/ztime.cpp

    data/zbinary.cpp
    data/zjson.cpp
    data/zxml.cpp

    #file/database.cpp
    file/zbmp.cpp
    file/zfile.cpp
    file/zimage.cpp
    file/zppm.cpp

    #lib/sqlite3.c
    lib/xxhash.c
    #lib/nowide_standalone/src/iostream.cpp

    #math/zexpression.cpp
    #math/zformula.cpp
    math/znumber.cpp
    #math/znumber-reference.cpp

    misc/zautobuffer.cpp
    misc/zencrypt.cpp
    misc/zmisc.cpp
    misc/zrandom.cpp
    misc/zuid.cpp

    net/zaddress.cpp
    net/zconnection.cpp
    net/zdatagramsocket.cpp
    net/zsocket.cpp
    net/zstreamsocket.cpp

    thread/zcondition.cpp
    #thread/zlock.cpp
    thread/zmutex.cpp
    thread/zthread.cpp
)

IF(LIBCHAOS_WINDOWS)
    SET(LibChaos_Core_SOURCES
        ${LibChaos_Core_SOURCES}
        #lib/StackWalker.cpp
    )
ENDIF()

### =================== CONFIG =================== ###

# Export Include Path
SET(LibChaos_Core_INCLUDE
    ${LibChaos_Core_SOURCE_DIR}
    ${LibChaos_Core_SOURCE_DIR}/abstract
    ${LibChaos_Core_SOURCE_DIR}/base
    ${LibChaos_Core_SOURCE_DIR}/data
    ${LibChaos_Core_SOURCE_DIR}/file
    ${LibChaos_Core_SOURCE_DIR}/lib
    ${LibChaos_Core_SOURCE_DIR}/net
    ${LibChaos_Core_SOURCE_DIR}/math
    ${LibChaos_Core_SOURCE_DIR}/misc
    ${LibChaos_Core_SOURCE_DIR}/thread
)

SET(LibChaos_Core_INCLUDE ${LibChaos_Core_INCLUDE} PARENT_SCOPE)

# Include Directories
INCLUDE_DIRECTORIES(
    ${LibChaos_Core_INCLUDE}
)

ADD_DEFINITIONS(-DZLOG_USE_THREAD)
#ADD_DEFINITIONS(-DZLOG_STD_MUTEX)

IF(IBERTY_DEMANGLE)
    ADD_DEFINITIONS(-DIBERTY_DEMANGLE)
ENDIF()

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

ADD_LIBRARY(chaos STATIC ${LibChaos_Core_SOURCES})
IF(LIBCHAOS_UNIX)
    TARGET_LINK_LIBRARIES(chaos pthread)
    IF(IBERTY_DEMANGLE)
        TARGET_LINK_LIBRARIES(chaos iberty)
    ENDIF()

    IF(FREEBSD)
        TARGET_LINK_LIBRARIES(chaos execinfo)
    ELSE()
        TARGET_LINK_LIBRARIES(chaos dl)
    ENDIF()
ELSEIF(LIBCHAOS_WINDOWS)
    IF(COMPILER_MINGW)
        TARGET_LINK_LIBRARIES(chaos pthread)
        TARGET_LINK_LIBRARIES(chaos ws2_32 dbghelp kernel32 psapi iphlpapi version)
    ELSEIF(COMPILER_MSVC)
        TARGET_LINK_LIBRARIES(chaos Ws2_32 iphlpapi)
    ENDIF()
ENDIF()
