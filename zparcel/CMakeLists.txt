## Project ZParcel CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(ZParcel)

SET(ZParcel_HEADERS ### =================== HEADERS =================== ###
    zparcel.h
    parcel-main.h
)
SET(ZParcel_SOURCES ### =================== SOURCES =================== ###
    zparcel.cpp
    parcel-main.cpp
    auto.cpp
    manual.cpp
)

### =================== CONFIG =================== ###

INCLUDE_DIRECTORIES(
    ${LibChaos_INCLUDE}
)

configure_build("ZParcel" ${LIBCHAOS_BUILD})
MESSAGE(STATUS ${BUILD_STRING})

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

# Parcelor
ADD_EXECUTABLE(zparcel ${ZParcel_SOURCES})
TARGET_LINK_LIBRARIES(zparcel chaos)
