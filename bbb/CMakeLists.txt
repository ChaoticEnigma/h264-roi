## Project LibChaosBBB CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(LibChaosBBB)

#ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../chaos ${CMAKE_CURRENT_BINARY_DIR}/chaos)

SET(LibChaosBBB_HEADERS ### =================== HEADERS =================== ###S
    zbbb.h
)
SET(LibChaosBBB_SOURCES ### =================== SOURCES =================== ###
    io.cpp
)

### =================== CONFIG =================== ###

SET(LibChaosBBB_INCLUDE ${LibChaosBBB_SOURCE_DIR} PARENT_SCOPE)

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}
    ${LibChaos_INCLUDE}
)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -ansi -std=c++11")

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)
ADD_DEFINITIONS(-DUNICODE -D_UNICODE)

ADD_LIBRARY(chaosbbb ${LibChaosBBB_SOURCES})#
TARGET_LINK_LIBRARIES(chaosbbb chaos)
