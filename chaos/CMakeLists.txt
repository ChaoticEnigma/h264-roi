## LibChaos Core CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(LibChaos)

#SET(IBERTY_DEMANGLE ON)

SET(WITH_PNG  FALSE)
SET(WITH_JPEG FALSE)

# Find zlib and libpng
FIND_PATH(ZLIB_INCLUDE NAMES zlib.h)
FIND_LIBRARY(ZLIB_LIB NAMES libzlibstatic.a)
FIND_PATH(LIBPNG_INCLUDE NAMES png.h)
FIND_LIBRARY(LIBPNG_LIB NAMES libpng.a)

IF(ZLIB_LIB AND ZLIB_INCLUDE AND LIBPNG_LIB AND LIBPNG_INCLUDE)
    MESSAGE(STATUS "Found ZLib and LibPNG")
    SET(WITH_PNG TRUE)
ENDIF()

# Find libjpeg
FIND_PATH(LIBJPEG_INCLUDE NAMES jpeglib.h)
FIND_LIBRARY(LIBJPEG_LIB NAMES libjpeg.a)

IF(LIBJPEG_LIB AND LIBJPEG_INCLUDE)
    MESSAGE(STATUS "Found LibJPEG")
    SET(WITH_JPEG TRUE)
ENDIF()

SET(LIBCHAOS_WITH_PNG  ${WITH_PNG}  PARENT_SCOPE)
SET(LIBCHAOS_WITH_JPEG ${WITH_JPEG} PARENT_SCOPE)

### =================== HEADERS =================== ###

SET(LibChaos_HEADERS
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
    base/ztime.h

    data/zallocator.h
    data/zarray.h
    data/zassoc.h
    data/zbinary.h
    data/zdata.h
    data/zdefaultstorage.h
    data/zfilestorage.h
    data/zlist.h
    data/zmap.h
    data/zpointer.h
    data/zqueue.h
    data/zstack.h

    file/database.h
    file/yimagebackend.h
    file/zbitmap.h
    file/zbmp.h
    file/zfile.h
    file/zimage.h
    file/zjpeg.h
    file/zoptions.h
    file/zpng.h
    file/zpdf.h
    file/zppm.h

    lib/fnv-5.0.3/fnv.h
    lib/sqlite3.h
    #lib/demangle.h

    math/zexpression.h
    math/zformula.h
    math/znumber.h
    math/zmath.h

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

### =================== SOURCES =================== ###

SET(LibChaos_SOURCES

    base/zclock.cpp
    base/zerror.cpp
    base/zexception.cpp
    base/zlog.cpp
    base/zlogworker.cpp
    base/ztime.cpp

    data/zbinary.cpp

    #file/database.cpp
    file/zbmp.cpp
    file/zfile.cpp
    file/zimage.cpp
    file/zjpeg.cpp
    file/zoptions.cpp
    file/zpng.cpp
    file/zpdf.cpp
    file/zppm.cpp

    lib/fnv-5.0.3/hash_64a.c
    #lib/sqlite3.c
    lib/xxhash.c
    #lib/nowide_standalone/src/iostream.cpp

    math/zexpression.cpp
    math/zformula.cpp
    math/znumber.cpp
    math/zmath.cpp
    #math/znumber-reference.cpp

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
    SET(LibChaos_SOURCES
        ${LibChaos_SOURCES}
        #lib/StackWalker.cpp
    )
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

IF(WITH_PNG)
    ADD_DEFINITIONS(-DLIBCHAOS_HAS_PNG -DPNG_STATIC)
    SET(EXTERN_INCLUDES ${EXTERN_INCLUDES} ${ZLIB_INCLUDE} ${LIBPNG_INCLUDE})
    SET(EXTERN_LIBRARIES ${EXTERN_LIBRARIES} ${LIBPNG_LIB} ${ZLIB_LIB})
ENDIF()

IF(WITH_JPEG)
    ADD_DEFINITIONS(-DLIBCHAOS_HAS_JPEG)
    SET(EXTERN_INCLUDES ${EXTERN_INCLUDES} ${LIBJPEG_INCLUDE})
    SET(EXTERN_LIBRARIES ${EXTERN_LIBRARIES} ${LIBJPEG_LIB})
ENDIF()

GatherSources(${LibChaos_HEADERS} ${LibChaos_SOURCES})

SET(LibChaos_INCLUDE
    .
    abstract
    base
    data
    file
    lib
    net
    math
    misc
    string
    thread
)

GatherIncludes(${LibChaos_INCLUDE})

# Include Directories
INCLUDE_DIRECTORIES(
    ${LibChaos_INCLUDE}
    ${EXTERN_INCLUDES}
    lib/fnv-5.0.3
)

IF(IBERTY_DEMANGLE)
    ADD_DEFINITIONS(-DIBERTY_DEMANGLE)
ENDIF()

### =================== BUILD =================== ###

ADD_LIBRARY(chaos ${LIBCHAOS_LINK} ${LibChaos_HEADERS} ${LibChaos_SOURCES})
TARGET_LINK_LIBRARIES(chaos ${EXTERN_LIBRARIES})

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
