## LibChaos Test CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(LibChaos_Test)

SET(LibChaosTest_SOURCES ### =================== TEST_SOURCES =================== ###
    libchaos_main.cpp

    pointer_test.cpp
    allocator_test.cpp
    storage_test.cpp

    binary_test.cpp

    array_test.cpp

    string_test.cpp
    path_test.cpp
    number_test.cpp

    thread_test.cpp

    file_test.cpp
    autobuffer_test.cpp
    json_test.cpp

    socket_test.cpp
    socket2_test.cpp

    sandbox.cpp
    error_test.cpp
)

IF(LIBCHAOS_EXTERN)
    SET(LibChaosTest_SOURCES ${LibChaosTest_SOURCES}
        png_test.cpp
    )
ENDIF()

### =================== CONFIG =================== ###

INCLUDE_DIRECTORIES(
    ${LibChaos_INCLUDE}
)

configure_build("Test" ${LIBCHAOS_BUILD} ${LIBCHAOS_PLATFORM})
SET(LibChaosTest_BUILD_STRING ${BUILD_STRING} PARENT_SCOPE)

ADD_DEFINITIONS(-DZLOG_USE_THREAD)
IF(DISABLE_LIBPNG)
    ADD_DEFINITIONS(-DDISABLE_LIBPNG)
ENDIF()

IF(LIBCHAOS_EXTERN)
    ADD_DEFINITIONS(-DLIBCHAOS_EXTERN)
ENDIF()

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

ADD_EXECUTABLE(testchaos ${LibChaosTest_SOURCES})
TARGET_LINK_LIBRARIES(testchaos chaos)
IF(LIBCHAOS_EXTERN)
    TARGET_LINK_LIBRARIES(testchaos chaosextern)
ENDIF()
