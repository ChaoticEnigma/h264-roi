## LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(CMAKE_GENERATOR "Unix Makefiles")

PROJECT(LibChaos)

SET(LibChaos_HEADERS ### =================== HEADERS =================== ###
    asar.h
    database.h
    sqlite3.h
    zfile.h
    zlog.h
    zlogworker.h
    zmutex.h
    zparcel.h
    zpath.h
    zstring.h
    zthread.h
)

SET(LibChaos_SOURCES ### =================== SOURCES =================== ###
    database.cpp
    sqlite3.c
    zfile.cpp
    zlog.cpp
    zlogworker.cpp
    zparcel.cpp
    zpath.cpp
    zstring.cpp
    zthread.cpp
)

INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIR}
)

ADD_DEFINITIONS(-g -DBUILDING -std=c++11)
ADD_DEFINITIONS(-DZLOG_USE_THREAD)

ADD_LIBRARY(chaos ${LibChaos_SOURCES})
TARGET_LINK_LIBRARIES(chaos pthread)
IF(NOT FREEBSD)
    TARGET_LINK_LIBRARIES(chaos dl)
ENDIF()

IF(LIBCHAOS_TEST)
    ADD_EXECUTABLE(test libchaos-main.cpp)
    TARGET_LINK_LIBRARIES(test chaos)
ENDIF()
