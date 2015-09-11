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

# Include Directories
GET_PROPERTY(LibChaos_Include GLOBAL PROPERTY LibChaosInclude)
GET_PROPERTY(LibChaosExtern_Include GLOBAL PROPERTY LibChaosExternInclude)
GET_PROPERTY(LibChaosUI_Include GLOBAL PROPERTY LibChaosUIInclude)
INCLUDE_DIRECTORIES(
    ${LibChaos_Include}
    ${LibChaosExtern_Include}
    ${LibChaosUI_Include}
)

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

### =================== TESTS =================== ###

ADD_TEST(NAME test_allocator    COMMAND $<TARGET_FILE:testchaos> allocator)
ADD_TEST(NAME test_pointer      COMMAND $<TARGET_FILE:testchaos> pointer)
ADD_TEST(NAME test_storage      COMMAND $<TARGET_FILE:testchaos> storage)
ADD_TEST(NAME test_binary       COMMAND $<TARGET_FILE:testchaos> binary)

ADD_TEST(NAME test_array        COMMAND $<TARGET_FILE:testchaos> array)
ADD_TEST(NAME test_assoc        COMMAND $<TARGET_FILE:testchaos> assoc)
ADD_TEST(NAME test_stack        COMMAND $<TARGET_FILE:testchaos> stack)
ADD_TEST(NAME test_map          COMMAND $<TARGET_FILE:testchaos> map)
ADD_TEST(NAME test_list         COMMAND $<TARGET_FILE:testchaos> list)
ADD_TEST(NAME test_queue        COMMAND $<TARGET_FILE:testchaos> queue)

ADD_TEST(NAME test_string       COMMAND $<TARGET_FILE:testchaos> string)
ADD_TEST(NAME test_path         COMMAND $<TARGET_FILE:testchaos> path)

ADD_TEST(NAME test_thread       COMMAND $<TARGET_FILE:testchaos> thread)
ADD_TEST(NAME test_mutex        COMMAND $<TARGET_FILE:testchaos> mutex)

ADD_TEST(NAME test_number       COMMAND $<TARGET_FILE:testchaos> number)
ADD_TEST(NAME test_uid          COMMAND $<TARGET_FILE:testchaos> uid)
ADD_TEST(NAME test_hash         COMMAND $<TARGET_FILE:testchaos> hash)


ADD_TEST(NAME test_file         COMMAND $<TARGET_FILE:testchaos> file)
ADD_TEST(NAME test_image        COMMAND $<TARGET_FILE:testchaos> image)
ADD_TEST(NAME test_png          COMMAND $<TARGET_FILE:testchaos> png)
ADD_TEST(NAME test_pdf          COMMAND $<TARGET_FILE:testchaos> pdf)

ADD_TEST(NAME test_autobuffer   COMMAND $<TARGET_FILE:testchaos> autobuffer)
ADD_TEST(NAME test_json         COMMAND $<TARGET_FILE:testchaos> json)
