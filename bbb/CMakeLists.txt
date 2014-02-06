## Project LibChaosBBB CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(LibChaosBBB)

#ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../chaos ${CMAKE_CURRENT_BINARY_DIR}/chaos)

SET(LibChaosBBB_HEADERS ### =================== HEADERS =================== ###S
    zbbb.h
)
SET(LibChaosBBB_SOURCES ### =================== SOURCES =================== ###
    io.cpp
)

### =================== CONFIG =================== ###

SET(LibChaosBBB_INCLUDE "${LibChaosBBB_SOURCE_DIR}" PARENT_SCOPE)

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}
    ${LibChaos_INCLUDE}
)

IF(DEBUG)
    ADD_DEFINITIONS(-DDEBUG)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    MESSAGE(STATUS "LibChaosBBB Debug Build Starting...")
ELSEIF(RELEASE)
    ADD_DEFINITIONS(-DRELEASE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    MESSAGE(STATUS "LibChaosBBB Release Build Starting...")
ELSE()
    MESSAGE(STATUS "LibChaosBBB Normal Build Starting...")
ENDIF()

IF(WINDOWS)
    ADD_DEFINITIONS(-DPLATFORM_WINDOWS -DCOMPILER_MINGW)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwin32 -mwindows")
ELSE()
    ADD_DEFINITIONS(-DPLATFORM_LINUX -DCOMPILER_GCC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")
ENDIF()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -ansi -std=c++11")

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)
ADD_DEFINITIONS(-DUNICODE -D_UNICODE)

ADD_LIBRARY(chaosbbb ${LibChaosBBB_SOURCES})#
TARGET_LINK_LIBRARIES(chaosbbb chaos)
