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
    json_test.cpp
    socket_test.cpp
    socket2_test.cpp
)

### =================== CONFIG =================== ###

INCLUDE_DIRECTORIES(
    ${LibChaos_Test_SOURCE_DIR}
    ${LibChaos_INCLUDE}
)

configure_build("Test" ${LIBCHAOS_BUILD} ${LIBCHAOS_PLATFORM})
SET(LibChaosTest_BUILD_STRING ${BUILD_STRING} PARENT_SCOPE)

ADD_DEFINITIONS(-DZLOG_USE_THREAD)

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

ADD_EXECUTABLE(testchaos ${LibChaosTest_SOURCES})
TARGET_LINK_LIBRARIES(testchaos chaos)
