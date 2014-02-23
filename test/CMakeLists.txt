## LibChaos Test CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(LibChaos_Test)

SET(LibChaosTest_SOURCES ### =================== TEST_SOURCES =================== ###
    libchaos_main.cpp
    array_test.cpp
    string_test.cpp
    file_test.cpp
    thread_test.cpp
    autobuffer_test.cpp
    error_test.cpp
    socket_test.cpp
    socket2_test.cpp
)

### =================== CONFIG =================== ###

INCLUDE_DIRECTORIES(
    ${LibChaos_Test_SOURCE_DIR}
    ${LibChaos_INCLUDE}
)

INCLUDE(../configure_build.cmake)
configure_build("Test" ${LIBCHAOS_BUILD} ${LIBCHAOS_PLATFORM})
SET(LibChaosTest_BUILD_STRING ${BUILD_STRING} PARENT_SCOPE)

ADD_DEFINITIONS(-DZLOG_USE_THREAD)
#ADD_DEFINITIONS(-DZLOG_STD_MUTEX)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -ansi -std=c++11")
#SET_SOURCE_FILES_PROPERTIES(sqlite3.c PROPERTIES COMPILE_FLAGS -std=c99)

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

ADD_EXECUTABLE(testchaos ${LibChaosTest_SOURCES})
TARGET_LINK_LIBRARIES(testchaos chaos)
