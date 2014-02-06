## LibChaos Test CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(LibChaos_Test)

SET(LibChaos_TEST_SOURCES ### =================== TEST_SOURCES =================== ###
    libchaos-main.cpp
    array_test.cpp
    string_test.cpp
    file_test.cpp
    thread_test.cpp
    autobuffer_test.cpp
    error_test.cpp
)

IF(BUILD_ZSOCKET)
    SET(LibChaos_Test_SOURCES ${LibChaos_TEST_SOURCES} socket_test.cpp)
ENDIF()

### =================== CONFIG =================== ###

INCLUDE_DIRECTORIES(
    ${LibChaos_Test_SOURCE_DIR}
    ${LibChaos_INCLUDE}
)

IF(DEBUG)
    ADD_DEFINITIONS(-DDEBUG)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    MESSAGE(STATUS "LibChaos Test Debug Build Starting...")
ELSEIF(RELEASE)
    ADD_DEFINITIONS(-DRELEASE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    MESSAGE(STATUS "LibChaos Test Release Build Starting...")
ELSE()
    MESSAGE(STATUS "LibChaos Test Normal Build Starting...")
ENDIF()

IF(WINDOWS)
    ADD_DEFINITIONS(-DPLATFORM_WINDOWS -DCOMPILER_MINGW)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwin32 -mwindows")
ELSE()
    ADD_DEFINITIONS(-DPLATFORM_LINUX -DCOMPILER_GCC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")
ENDIF()

ADD_DEFINITIONS(-DZLOG_USE_THREAD)
#ADD_DEFINITIONS(-DZLOG_STD_MUTEX)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -ansi -std=c++11")
#SET_SOURCE_FILES_PROPERTIES(sqlite3.c PROPERTIES COMPILE_FLAGS -std=c99)

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

ADD_EXECUTABLE(testchaos ${LibChaos_Test_SOURCES})
TARGET_LINK_LIBRARIES(testchaos chaos)

