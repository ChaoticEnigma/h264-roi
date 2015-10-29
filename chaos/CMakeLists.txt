## LibChaos Core CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(LibChaos)

#SET(IBERTY_DEMANGLE ON)

### =================== SOURCES =================== ###

SET(LibChaos_SOURCES
    ztypes.h

    abstract/yindexedaccess.h
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
    base/zclock.cpp
    base/zerror.h
    base/zerror.cpp
    base/zexception.h
    base/zexception.cpp
    base/zlog.h
    base/zlog.cpp
    base/zlogworker.h
    base/zlogworker.cpp
    base/ztime.h
    base/ztime.cpp

    data/zallocator.h
    data/zarray.h
    data/zassoc.h
    data/zbinary.h
    data/zbinary.cpp
    data/zdata.h
    data/zdefaultstorage.h
    data/zfilestorage.h
    data/zlist.h
    data/zmap.h
    data/zpointer.h
    data/zqueue.h
    data/zset.h
    data/zstack.h

    file/database.h
    #file/database.cpp
    file/yimagebackend.h
    file/zbitmap.h
    file/zfile.h
    file/zfile.cpp
    file/zimage.h
    file/zimage.cpp
    file/zoptions.h
    file/zoptions.cpp
    file/zpdf.h
    file/zpdf.cpp
    file/image/zbmp.h
    file/image/zbmp.cpp
    file/image/zjpeg.h
    #file/image/zjpeg.cpp
    file/image/zpng.h
    #file/image/zpng.cpp
    file/image/zppm.h
    file/image/zppm.cpp
    file/image/zwebp.h

    #lib/demangle.h
    lib/fnv-5.0.3/fnv.h
    lib/fnv-5.0.3/hash_64a.c
    #lib/nowide_standalone/src/iostream.cpp
    lib/sqlite3.h
    #lib/sqlite3.c
    lib/xxhash.c

    math/zexpression.h
    math/zexpression.cpp
    math/zformula.h
    math/zformula.cpp
    math/znumber.h
    math/znumber.cpp
    #math/znumber-reference.cpp
    math/zmath.h
    math/zmath.cpp

    misc/zencrypt.h
    misc/zencrypt.cpp
    misc/zhash.h
    misc/zhash.cpp
    misc/zrandom.h
    misc/zrandom.cpp
    misc/zret.h
    misc/zuid.h
    misc/zuid.cpp

    net/zaddress.h
    net/zaddress.cpp
    net/zconnection.h
    net/zconnection.cpp
    net/zdatagramsocket.h
    net/zdatagramsocket.cpp
    net/zsocket.h
    net/zsocket.cpp
    net/zstreamsocket.h
    net/zstreamsocket.cpp

    string/zjson.h
    string/zjson.cpp
    string/zpath.h
    string/zpath.cpp
    string/zstring.h
    string/zstring.cpp
    string/zstring-encode.cpp
    string/zxml.h
    string/zxml.cpp

    thread/zcondition.h
    thread/zcondition.cpp
    thread/zlock.h
    #thread/zlock.cpp
    thread/zmutex.h
    thread/zmutex.cpp
    thread/zthread.h
    thread/zthread.cpp
)

SET(LibChaos_PNG_SOURCES
    file/image/zpng.cpp
)
SET(LibChaos_JPEG_SOURCES
    file/image/zjpeg.cpp
)
SET(LibChaos_WEBP_SOURCES
    file/image/zwebp.cpp
)

SET(LibChaos_WIN32_SOURCES
    lib/StackWalker.cpp
)

IF(LIBCHAOS_WITH_PNG)
    SET(LibChaos_SOURCES ${LibChaos_SOURCES} ${LibChaos_PNG_SOURCES})
ENDIF()
IF(LIBCHAOS_WITH_JPEG)
    SET(LibChaos_SOURCES ${LibChaos_SOURCES} ${LibChaos_JPEG_SOURCES})
ENDIF()
IF(LIBCHAOS_WITH_WEBP)
    SET(LibChaos_SOURCES ${LibChaos_SOURCES} ${LibChaos_WEBP_SOURCES})
ENDIF()

IF(LIBCHAOS_PLATFORM_WINDOWS)
    #SET(LibChaos_SOURCES ${LibChaos_SOURCES} ${LibChaos_WIN32_SOURCES})
ENDIF()

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

GatherSources(
    ${LibChaos_HEADERS}
    ${LibChaos_SOURCES}
    ${LibChaos_PNG_SOURCES}
    ${LibChaos_JPEG_SOURCES}
    ${LibChaos_WEBP_SOURCES}
    ${LibChaos_WIN32_SOURCES}
)

SET(LibChaos_INCLUDE
    .
    abstract
    base
    data
    file
    file/image
    lib
    math
    misc
    net
    string
    thread
)

GatherIncludes(${LibChaos_INCLUDE})

# Include Directories
INCLUDE_DIRECTORIES(
    ${LibChaos_INCLUDE}
    lib/fnv-5.0.3
)

IF(LIBCHAOS_WITH_PNG)
    INCLUDE_DIRECTORIES(${ZLIB_INCLUDE} ${LIBPNG_INCLUDE})
    ADD_DEFINITIONS(-DPNG_STATIC)
ENDIF()
IF(LIBCHAOS_WITH_JPEG)
    INCLUDE_DIRECTORIES(${LIBJPEG_INCLUDE})
ENDIF()
IF(LIBCHAOS_WITH_WEBP)
    INCLUDE_DIRECTORIES(${LIBWEBP_INCLUDE})
ENDIF()

IF(IBERTY_DEMANGLE)
    ADD_DEFINITIONS(-DIBERTY_DEMANGLE)
ENDIF()

### =================== BUILD =================== ###

ADD_LIBRARY(chaos ${LIBCHAOS_LINK} ${LibChaos_HEADERS} ${LibChaos_SOURCES})

IF(LIBCHAOS_WITH_PNG)
    TARGET_LINK_LIBRARIES(chaos ${LIBPNG_LIB} ${ZLIB_LIB})
ENDIF()
IF(LIBCHAOS_WITH_JPEG)
    TARGET_LINK_LIBRARIES(chaos ${LIBJPEG_LIB})
ENDIF()
IF(LIBCHAOS_WITH_WEBP)
    TARGET_LINK_LIBRARIES(chaos ${LIBWEBP_LIB})
ENDIF()


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
