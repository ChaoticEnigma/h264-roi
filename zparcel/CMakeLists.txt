## Project ZParcel CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

PROJECT(ZParcel)

SET(ZParcel_HEADERS ### =================== HEADERS =================== ###
    main.h
    zparcel.h
    zparceltypes.h
    zparcelconvert.h
    zparcel4parser.h

)
SET(ZParcel_SOURCES ### =================== SOURCES =================== ###
    ${ZParcel_HEADERS}
    main.cpp
    zparcel.cpp
    zparcelconvert.cpp
    zparcel4parser.cpp
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
