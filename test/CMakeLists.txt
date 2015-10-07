## LibChaos Test CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(LibChaos_Test)

### =================== SOURCES =================== ###

SET(LibChaos_Test_HEADERS
    test.h
)

### =================== SOURCES =================== ###

SET(LibChaos_Test_SOURCES
    test.cpp

    pointer_test.cpp
    allocator_test.cpp
    storage_test.cpp
    binary_test.cpp

    array_test.cpp
    list_test.cpp

    string_test.cpp
    path_test.cpp
    json_test.cpp
    number_test.cpp

    thread_test.cpp

    misc_test.cpp

    file_test.cpp
    image_test.cpp
    pdf_test.cpp

    socket_test.cpp
    socket2_test.cpp

    sandbox.cpp
    error_test.cpp
)

### =================== CONFIG =================== ###

# Export Properties
GatherSources(${LibChaos_Test_HEADERS} ${LibChaos_Test_SOURCES} ${LibChaos_Test_PNG_SOURCES})

# Include Directories
GET_PROPERTY(LibChaos_Include GLOBAL PROPERTY LibChaosInclude)
GET_PROPERTY(LibChaosExtern_Include GLOBAL PROPERTY LibChaosExternInclude)
GET_PROPERTY(LibChaosUI_Include GLOBAL PROPERTY LibChaosUIInclude)
INCLUDE_DIRECTORIES(
    ${LibChaos_Include}
    ${LibChaosExtern_Include}
    ${LibChaosUI_Include}
)

### =================== BUILD =================== ###

ADD_EXECUTABLE(testchaos ${LIBCHAOS_TEST_ALL} ${LibChaos_Test_HEADERS} ${LibChaos_Test_SOURCES})
TARGET_LINK_LIBRARIES(testchaos chaos)

### =================== TESTS =================== ###

ADD_TEST(NAME "Allocator" CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> allocator)
ADD_TEST(NAME "Pointer"   CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> pointer)
ADD_TEST(NAME "Storage"   CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> storage)
ADD_TEST(NAME "Binary"    CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> binary)

ADD_TEST(NAME "Array"     CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> array)
ADD_TEST(NAME "Assoc"     CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> assoc)
ADD_TEST(NAME "Stack"     CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> stack)
ADD_TEST(NAME "Map"       CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> map)
ADD_TEST(NAME "List"      CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> list)
ADD_TEST(NAME "Queue"     CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> queue)

ADD_TEST(NAME "String"    CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> string)
ADD_TEST(NAME "Path"      CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> path)
ADD_TEST(NAME "JSON"      CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> json)
ADD_TEST(NAME "Number"    CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> number)

ADD_TEST(NAME "Hash"      CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> hash)
ADD_TEST(NAME "UID"       CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> uid)

ADD_TEST(NAME "Thread"    CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> thread)
ADD_TEST(NAME "Mutex"     CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> mutex)

ADD_TEST(NAME "File"      CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> file)
ADD_TEST(NAME "Image"     CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> image)
ADD_TEST(NAME "PDF"       CONFIGURATIONS testchaos COMMAND $<TARGET_FILE:testchaos> pdf)

