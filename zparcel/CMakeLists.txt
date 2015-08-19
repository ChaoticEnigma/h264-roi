## ZParcel CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

PROJECT(ZParcel)

SET(ZParcel_HEADERS ### =================== HEADERS =================== ###
    main.h
    zparcel.h
    #zparceltypes.h
    #zparcelconvert.h
    zparcel4page.h
    zparcel4parser.h

)
SET(ZParcel_SOURCES ### =================== SOURCES =================== ###
    ${ZParcel_HEADERS}
    main.cpp
    zparcel.cpp
    #zparcelconvert.cpp
    zparcel4page.cpp
    zparcel4parser.cpp
)

### =================== CONFIG =================== ###

GET_PROPERTY(LibChaos_Include GLOBAL PROPERTY LibChaosInclude)
INCLUDE_DIRECTORIES(
    ${LibChaos_Include}
)

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

# Parcelor
ADD_EXECUTABLE(zparcel ${LIBCHAOS_ZPARCEL_ALL} ${ZParcel_SOURCES})
TARGET_LINK_LIBRARIES(zparcel chaos)
