## LibChaos Test CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

PROJECT(LibChaos_Test)

SET(LibChaos_Test_HEADERS ### =================== SOURCES =================== ###
    test.h
)

SET(LibChaos_Test_SOURCES ### =================== SOURCES =================== ###
    test.cpp

    pointer_test.cpp
    allocator_test.cpp
    storage_test.cpp

    binary_test.cpp

    array_test.cpp
    list_test.cpp
    iterator_test.cpp

    string_test.cpp
    path_test.cpp
    number_test.cpp

    thread_test.cpp

    file_test.cpp
    autobuffer_test.cpp
    json_test.cpp
    pdf_test.cpp

    misc_test.cpp

    socket_test.cpp
    socket2_test.cpp

    sandbox.cpp
    error_test.cpp
)

IF(ENABLE_ZPNG)
    SET(LibChaos_Test_SOURCES ${LibChaos_Test_SOURCES}
        image_test.cpp
        png_test.cpp
    )
ENDIF()

### =================== CONFIG =================== ###

INCLUDE_DIRECTORIES(
    ${LibChaos_INCLUDE}
)

ADD_DEFINITIONS(-DZLOG_USE_THREAD)

# Set preprocessor defines
IF(LibChaos_Has_PNG)
    ADD_DEFINITIONS(-DTEST_ZPNG)
ENDIF()

### =================== BUILD =================== ###

ADD_EXECUTABLE(testchaos ${LIBCHAOS_TEST_ALL} ${LibChaos_Test_HEADERS} ${LibChaos_Test_SOURCES})
TARGET_LINK_LIBRARIES(testchaos chaos)
IF(LIBCHAOS_HAS_PNG)
    TARGET_LINK_LIBRARIES(testchaos chaosextern)
ENDIF()
