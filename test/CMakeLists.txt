#f # LibChaos Test CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(LibChaos_Test)

### =================== SOURCES =================== ###

SET(LibChaos_Test_SOURCES
    tests.h
    main.cpp
    sandbox.cpp

    test_pointer.cpp
    test_allocator.cpp
    test_storage.cpp
    test_binary.cpp

    test_array.cpp
    test_list.cpp
    test_iterator.cpp

    test_graph.cpp

    test_string.cpp
    test_path.cpp
    test_json.cpp
    test_number.cpp

    test_thread.cpp

    test_misc.cpp
    test_hash.cpp

    test_file.cpp
    test_image.cpp
    test_pdf.cpp

    test_socket.cpp

    test_error.cpp
)

### =================== CONFIG =================== ###

# Export Properties
CollectSources(${LibChaos_Test_HEADERS} ${LibChaos_Test_PNG_SOURCES})

# Include Directories
GET_PROPERTY(LibChaos_Include GLOBAL PROPERTY LibChaosInclude)
GET_PROPERTY(LibChaosExtern_Include GLOBAL PROPERTY LibChaosExternInclude)
GET_PROPERTY(LibChaosUI_Include GLOBAL PROPERTY LibChaosUIInclude)
INCLUDE_DIRECTORIES(
    ${LibChaos_Include}
    ${LibChaosExtern_Include}
    ${LibChaosUI_Include}
)

# Download test images
IF(LIBCHAOS_TEST AND LIBCHAOS_WITH_PNG)
    DownloadArchive("https://imagetestsuite.googlecode.com/files/imagetestsuite-png-1.01.tar.gz" testpng.tar.gz)
    DownloadArchive("http://www.schaik.com/pngsuite/PngSuite-2013jan13.tgz" pngsuite.tar.gz)
ENDIF()
IF(LIBCHAOS_TEST AND LIBCHAOS_WITH_JPEG)
    DownloadFile("http://www.gstatic.com/webp/gallery/4.jpg" tree.jpg)
    DownloadArchive("https://imagetestsuite.googlecode.com/files/imagetestsuite-jpg-1.00.tar.gz" testjpeg.tar.gz)
ENDIF()
IF(LIBCHAOS_TEST AND LIBCHAOS_WITH_WEBP)
    DownloadFile("https://www.gstatic.com/webp/gallery3/3_webp_ll.webp" dice.webp)
ENDIF()

### =================== BUILD =================== ###

ADD_EXECUTABLE(testchaos ${LibChaos_Test_HEADERS} ${LibChaos_Test_SOURCES})
LibChaos_Configure_Target(testchaos)

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

