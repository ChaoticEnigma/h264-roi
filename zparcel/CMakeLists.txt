## Project ZParcel CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

PROJECT(ZParcel)

SET(ZParcel_HEADERS ### =================== HEADERS =================== ###
    zparcel-parser.h
    zparcel.h
    zparcel4-parser.h

    parcel-main.h
)
SET(ZParcel_SOURCES ### =================== SOURCES =================== ###
    ${ZParcel_HEADERS}

    #zparcel.cpp
    zparcel4-parser.cpp

    parcel-main.cpp
)

### =================== CONFIG =================== ###

INCLUDE_DIRECTORIES(
    ${LibChaos_INCLUDE}
)

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

# Parcelor
ADD_EXECUTABLE(zparcel ${ZParcel_SOURCES})
TARGET_LINK_LIBRARIES(zparcel chaos)
