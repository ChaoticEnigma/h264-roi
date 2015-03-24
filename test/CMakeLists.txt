## LibChaos Test CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(LibChaos_Test)

SET(LibChaosTest_SOURCES ### =================== TEST_SOURCES =================== ###
    libchaos_main.cpp

    pointer_test.cpp
    allocator_test.cpp
    storage_test.cpp

    binary_test.cpp

    array_test.cpp
    list_test.cpp

    string_test.cpp
    path_test.cpp
    number_test.cpp

    thread_test.cpp

    file_test.cpp
    autobuffer_test.cpp
    json_test.cpp

    misc_test.cpp

    socket_test.cpp
    socket2_test.cpp

    sandbox.cpp
    error_test.cpp
)

IF(ENABLE_ZPNG)
    SET(LibChaosTest_SOURCES ${LibChaosTest_SOURCES} png_test.cpp)
ENDIF()

### =================== CONFIG =================== ###

INCLUDE_DIRECTORIES(
    ${LibChaos_INCLUDE}
)

ADD_DEFINITIONS(-DZLOG_USE_THREAD)

# Set preprocessor defines
IF(ENABLE_ZPNG)
    ADD_DEFINITIONS(-DTEST_ZPNG)
ENDIF()

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

ADD_EXECUTABLE(testchaos ${LibChaosTest_SOURCES})
TARGET_LINK_LIBRARIES(testchaos chaos)
IF(ENABLE_ZPNG)
    TARGET_LINK_LIBRARIES(testchaos chaosextern)
ENDIF()
