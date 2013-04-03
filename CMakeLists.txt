## LibChaos CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(CMAKE_GENERATOR "Unix Makefiles")

PROJECT(LibChaos)

SET(LibChaos_HEADERS ### =================== HEADERS =================== ###
    zarray.h
    zstring.h
)

SET(LibChaos_SOURCES ### =================== SOURCES =================== ###
    #zarray.cpp
    zstring.cpp
)

INCLUDE_DIRECTORIES(
    ./
)

ADD_DEFINITIONS(-D BUILDING -g)

ADD_LIBRARY(chaos ${LibChaos_SOURCES})

ADD_EXECUTABLE(test main.cpp)
TARGET_LINK_LIBRARIES(test chaos)
