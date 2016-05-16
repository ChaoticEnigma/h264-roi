## ZParcel CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(ZParcel)

### =================== HEADERS =================== ###

SET(ZParcel_HEADERS
    main.h
    zparcel.h
    zparcelrecord.h
    #zparceltypes.h
    #zparcelconvert.h
    zparcel4page.h
    zparcel4parser.h
)

### =================== SOURCES =================== ###

SET(ZParcel_SOURCES
    ${ZParcel_HEADERS}
    main.cpp
    zparcel.cpp
    zparcelrecord.cpp
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

ADD_EXECUTABLE(zparcel ${LIBCHAOS_ZPARCEL_ALL} ${ZParcel_SOURCES})
LibChaos_Configure_Target(zparcel)

### =================== TESTS =================== ###

ADD_TEST(NAME "Create Parcel" CONFIGURATIONS zparcel COMMAND $<TARGET_FILE:zparcel> create test.parcel)
ADD_TEST(NAME "List Parcel" CONFIGURATIONS zparcel COMMAND $<TARGET_FILE:zparcel> list test.parcel)
