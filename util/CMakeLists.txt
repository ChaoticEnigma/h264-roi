## LibChaos Utility CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

PROJECT(LibChaos_Utils)

### =================== CONFIG =================== ###

OPTION(LIBCHAOS_UTIL_EXTENSION_FIXER          "Build LibChaos Extension Fixer"        OFF )
OPTION(LIBCHAOS_UTIL_AUDIOBOOK_SORTER         "Build LibChaos Audiobook Sorter"       OFF )
OPTION(LIBCHAOS_UTIL_DUPLICATE_FINDER         "Build LibChaos Duplicate Finder"       OFF )
OPTION(LIBCHAOS_UTIL_IMAGE_DOWNLOADER         "Build LibChaos Image Downloader"       ON  )
OPTION(LIBCHAOS_UTIL_IMAGE_LIBRARY_MANAGER    "Build LibChaos Image Library Manager"  ON  )
OPTION(LIBCHAOS_UTIL_IMAGE_REVERSE_SEARCH     "Build LibChaos Image Reverse Search"   OFF )

INCLUDE_DIRECTORIES(
    ${LibChaos_Utils_SOURCE_DIR}
    ${LibChaos_INCLUDE}
)

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

# Extension Fixer
IF(LIBCHAOS_UTIL_EXTENSION_FIXER)
    ADD_EXECUTABLE(extensionfixer ${LIBCHAOS_UTIL_ALL} extensionfixer.cpp)
    TARGET_LINK_LIBRARIES(extensionfixer chaos)
ENDIF()

# Audiobook Sorter
IF(LIBCHAOS_UTIL_AUDIOBOOK_SORTER)
    ADD_EXECUTABLE(audiobooksorter ${LIBCHAOS_UTIL_ALL} audiobooksorter.cpp)
    TARGET_LINK_LIBRARIES(audiobooksorter chaos)
ENDIF()

# Duplicate Finder
IF(LIBCHAOS_UTIL_DUPLICATE_FINDER)
    ADD_EXECUTABLE(duplicatefinder ${LIBCHAOS_UTIL_ALL} duplicatefinder.cpp)
    TARGET_LINK_LIBRARIES(duplicatefinder chaos)
ENDIF()

# Image Downloader
IF(LIBCHAOS_UTIL_IMAGE_DOWNLOADER)
    ADD_EXECUTABLE(imagedownloader ${LIBCHAOS_UTIL_ALL}
        imagedownloader/imagedownloader.cpp

        # Other Files
        imagedownloader/.htaccess
        imagedownloader/save.js
        imagedownloader/save.php
        imagedownloader/save-bookmark.txt
    )
    TARGET_LINK_LIBRARIES(imagedownloader chaos)
ENDIF()

# Image Library Manager
IF(LIBCHAOS_UTIL_IMAGE_LIBRARY_MANAGER)
    ADD_EXECUTABLE(imagelibrarymanger ${LIBCHAOS_UTIL_ALL} imagelibrarymanager.cpp)
    TARGET_LINK_LIBRARIES(imagelibrarymanger chaos)
ENDIF()

# Image Reverse Search
IF(LIBCHAOS_UTIL_IMAGE_REVERSE_SEARCH)
    ADD_EXECUTABLE(imagereversesearch ${LIBCHAOS_UTIL_ALL} imagereversesearch.cpp)
    #TARGET_LINK_LIBRARIES(imagereversesearch chaos ssl ssh2 ssh2dll crypto z rtmp curl curldll)
    ADD_DEFINITIONS(-DCURL_STATICLIB)
    TARGET_LINK_LIBRARIES(imagereversesearch chaos crypto ssl z curl)
    #SET_TARGET_PROPERTIES(imagereversesearch PROPERTIES LINK_FLAGS -Wl,-static)
ENDIF()
