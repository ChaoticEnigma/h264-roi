## LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

PROJECT(LibChaos_Core)

OPTION(IBERTY_DEMANGLE "Use libiberty to demangle symbols" OFF)

#IF(LIBCHAOS_PLATFORM_LINUX AND LIBCHAOS_COMPILER_GCC AND LIBCHAOS_BUILD_DEBUG)
#    SET(IBERTY_DEMANGLE ON)
#ENDIF()

SET(LibChaos_Core_HEADERS ### =================== HEADERS =================== ###
    ztypes.h

    abstract/yindexedaccess.h
    abstract/ypushpopaccess.h
    abstract/zaccessor.h
    abstract/zbase.h
    abstract/zhashable.h
    abstract/ziterable.h
    abstract/zposition.h
    abstract/zreader.h
    abstract/zserializer.h
    abstract/zstorage.h
    abstract/zwriter.h

    base/zclock.h
    base/zerror.h
    base/zexception.h
    base/ziterator.h
    base/zlog.h
    base/zlogworker.h
    #base/zpath.h
    #base/zstring.h
    base/ztime.h

    data/zallocator.h
    data/zarray.h
    data/zassoc.h
    data/zbinary.h
    data/zdata.h
    data/zdefaultstorage.h
    data/zfilestorage.h
    #data/zjson.h
    data/zlist.h
    data/zmap.h
    data/zpointer.h
    data/zqueue.h
    data/zstack.h
    #data/zxml.h

    file/database.h
    file/zbitmap.h
    file/zbmp.h
    file/zfile.h
    file/zimage.h
    file/zoptions.h
    file/zppm.h

    lib/fnv-5.0.3/fnv.h
    lib/sqlite3.h
    #lib/demangle.h

    math/zexpression.h
    math/zformula.h
    math/znumber.h

    misc/zautobuffer.h
    misc/zautobufferunit.h
    misc/zencrypt.h
    misc/zhash.h
    misc/zrandom.h
    misc/zret.h
    misc/zuid.h

    net/zaddress.h
    net/zconnection.h
    net/zdatagramsocket.h
    net/zsocket.h
    net/zstreamsocket.h

    string/zjson.h
    string/zpath.h
    string/zstring.h
    string/zxml.h

    thread/zcondition.h
    thread/zlock.h
    thread/zmutex.h
    thread/zthread.h
)

SET(LibChaos_Core_SOURCES ### =================== SOURCES =================== ###

    base/zclock.cpp
    base/zerror.cpp
    base/zexception.cpp
    base/zlog.cpp
    base/zlogworker.cpp
    #base/zpath.cpp
    #base/zstring.cpp
    #base/zstring_encode.cpp
    base/ztime.cpp

    data/zbinary.cpp
    #data/zjson.cpp
    #data/zxml.cpp

    #file/database.cpp
    file/zbmp.cpp
    file/zfile.cpp
    file/zimage.cpp
    file/zoptions.cpp
    file/zppm.cpp

    lib/fnv-5.0.3/hash_64a.c
    #lib/sqlite3.c
    lib/xxhash.c
    #lib/nowide_standalone/src/iostream.cpp

    math/zexpression.cpp
    math/zformula.cpp
    math/znumber.cpp
    #math/znumber-reference.cpp

    misc/zautobuffer.cpp
    misc/zencrypt.cpp
    misc/zhash.cpp
    misc/zrandom.cpp
    misc/zuid.cpp

    net/zaddress.cpp
    net/zconnection.cpp
    net/zdatagramsocket.cpp
    net/zsocket.cpp
    net/zstreamsocket.cpp

    string/zjson.cpp
    string/zpath.cpp
    string/zstring.cpp
    string/zstring-encode.cpp
    string/zxml.cpp

    thread/zcondition.cpp
    #thread/zlock.cpp
    thread/zmutex.cpp
    thread/zthread.cpp
)

IF(LIBCHAOS_PLATFORM_WINDOWS)
    SET(LibChaos_Core_SOURCES
        ${LibChaos_Core_SOURCES}
        #lib/StackWalker.cpp
    )
ENDIF()

SET(LibChaos_Core_FILES ${LibChaos_Core_HEADERS} ${LibChaos_Core_SOURCES} PARENT_SCOPE)

### =================== CONFIG =================== ###

# Visual Studio Project Filters
SOURCE_GROUP(""         FILES ztypes.h)
SOURCE_GROUP(abstract   REGULAR_EXPRESSION "abstract/.*")
SOURCE_GROUP(base       REGULAR_EXPRESSION "base/.*")
SOURCE_GROUP(data       REGULAR_EXPRESSION "data/.*")
SOURCE_GROUP(file       REGULAR_EXPRESSION "file/.*")
#SOURCE_GROUP(lib        REGULAR_EXPRESSION "lib/.*")
SOURCE_GROUP(math       REGULAR_EXPRESSION "math/.*")
SOURCE_GROUP(misc       REGULAR_EXPRESSION "misc/.*")
SOURCE_GROUP(net        REGULAR_EXPRESSION "net/.*")
SOURCE_GROUP(string     REGULAR_EXPRESSION "string/.*")
SOURCE_GROUP(thread     REGULAR_EXPRESSION "thread/.*")

# Export Include Path
SET(LibChaos_Core_INCLUDE
    #${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/abstract
    ${CMAKE_CURRENT_SOURCE_DIR}/base
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    ${CMAKE_CURRENT_SOURCE_DIR}/file
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/net
    ${CMAKE_CURRENT_SOURCE_DIR}/math
    ${CMAKE_CURRENT_SOURCE_DIR}/misc
    ${CMAKE_CURRENT_SOURCE_DIR}/string
    ${CMAKE_CURRENT_SOURCE_DIR}/thread
)

SET(LibChaos_Core_INCLUDE ${LibChaos_Core_INCLUDE} PARENT_SCOPE)

# Include Directories
INCLUDE_DIRECTORIES(
    ${LibChaos_Core_INCLUDE}
    lib/fnv-5.0.3
)

ADD_DEFINITIONS(-DZLOG_USE_THREAD)
#ADD_DEFINITIONS(-DZLOG_STD_MUTEX)

IF(IBERTY_DEMANGLE)
    ADD_DEFINITIONS(-DIBERTY_DEMANGLE)
ENDIF()

# Configure
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/ztypes.h" "${CMAKE_CURRENT_BINARY_DIR}/ztypes.h")

### =================== BUILD =================== ###

ADD_LIBRARY(chaos ${LIBCHAOS_LINK} ${LibChaos_Core_HEADERS} ${LibChaos_Core_SOURCES})
IF(LIBCHAOS_PLATFORM_WINDOWS)
    IF(LIBCHAOS_COMPILER_MSVC)
        TARGET_LINK_LIBRARIES(chaos Ws2_32 iphlpapi)
    ELSE()
        TARGET_LINK_LIBRARIES(chaos pthread)
        TARGET_LINK_LIBRARIES(chaos ws2_32 dbghelp kernel32 psapi iphlpapi version)
    ENDIF()
ELSE()
    TARGET_LINK_LIBRARIES(chaos pthread)
    IF(IBERTY_DEMANGLE)
        TARGET_LINK_LIBRARIES(chaos iberty)
    ENDIF()

    IF(LIBCHAOS_PLATFORM_FREEBSD)
        TARGET_LINK_LIBRARIES(chaos execinfo)
    ELSE()
        TARGET_LINK_LIBRARIES(chaos dl)
    ENDIF()
ENDIF()
