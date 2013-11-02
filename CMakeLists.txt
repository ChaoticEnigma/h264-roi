## LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(CMAKE_GENERATOR "Unix Makefiles")

PROJECT(LibChaos)

SET(LibChaos_HEADERS ### =================== HEADERS =================== ###
    asar.h
    database.h
    sqlite3.h
    zarray.h
    zassoc.h
    zbinary.h
    zfile.h
    zjson.h
    zlog.h
    zlogworker.h
    zmutex.h
    #zparcel.h
    zpath.h
    zstring.h
    zthread.h
)

SET(LibChaos_SOURCES ### =================== SOURCES =================== ###
    database.cpp
    sqlite3.c
    zbinary.cpp
    zfile.cpp
    zjson.cpp
    zlog.cpp
    zlogworker.cpp
    #zparcel.cpp
    zpath.cpp
    zstring.cpp
    zthread.cpp
)

### =================== CONFIG =================== ###

SET(LibChaos_INCLUDE "${LibChaos_SOURCE_DIR}" PARENT_SCOPE)

INCLUDE_DIRECTORIES(
#    ${Boost_INCLUDE_DIR}
)

IF(WIN32)
    SET(WINDOWS TRUE)
ELSE()
    SET(UNIX TRUE)
ENDIF()

IF(DEBUG)
    ADD_DEFINITIONS(-g)
    MESSAGE(STATUS "LibChaos Debug Build Starting...")
ELSEIF(RELEASE)
    ADD_DEFINITIONS(-O3)
    MESSAGE(STATUS "LibChaos Release Build Starting...")
ELSE()
    MESSAGE(STATUS "LibChaos Normal Build Starting...")
ENDIF()

IF(WINDOWS)
    ADD_DEFINITIONS(-DPLATFORM_WINDOWS -DCOMPILER_MINGW)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwin32 -mwindows")
ELSE()
    ADD_DEFINITIONS(-DPLATFORM_LINUX -DCOMPILER_GCC)
ENDIF()

ADD_DEFINITIONS(-DZLOG_USE_THREAD)

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
    ADD_EXECUTABLE(testchaos libchaos-main.cpp)
    TARGET_LINK_LIBRARIES(testchaos chaos)
ENDIF()
