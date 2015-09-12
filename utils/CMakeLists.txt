## LibChaos Utilities CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(LibChaosUtils)

### =================== SOURCES =================== ###

SET(LibChaosUtils_ExtensionFixer_SOURCES
    extensionfixer.cpp
)

SET(LibChaosUtils_AudiobookSorter_SOURCES
    audiobooksorter.cpp
)

SET(LibChaosUtils_DuplicateFinder_SOURCES
    duplicatefinder.cpp
)

SET(LibChaosUtils_ImageDownloader_SOURCES
    imagedownloader.cpp
)

SET(LibChaosUtils_ImageLibraryManager_SOURCES
    imagelibrarymanager.cpp
)

SET(LibChaosUtils_ImageReverseSearch_SOURCES
    imagereversesearch.cpp
)

SET(LibChaosUtils_FILES
    ${LibChaos_Utils_ExtensionFixer_SOURCES}
    ${LibChaos_Utils_AudiobookSorter_SOURCES}
    ${LibChaos_Utils_DuplicateFinder_SOURCES}
    ${LibChaos_Utils_ImageDownloader_SOURCES}
    ${LibChaos_Utils_ImageLibraryManager_SOURCES}
    ${LibChaos_Utils_ImageReverseSearch_SOURCES}
    PARENT_SCOPE
)

### =================== CONFIG =================== ###

GET_PROPERTY(LibChaos_Include GLOBAL PROPERTY LibChaosInclude)
INCLUDE_DIRECTORIES(
    ${LibChaos_Include}
    ${LibChaosUtils_SOURCE_DIR}
)

### =================== BUILD =================== ###

# Extension Fixer
IF(NOT LIBCHAOS_UTIL_EXTENSION_FIXER)
    SET(LIBCHAOS_UTIL_EXTENSION_FIXER_ALL EXCLUDE_FROM_ALL)
ENDIF()
ADD_EXECUTABLE(extensionfixer ${LIBCHAOS_UTIL_EXTENSION_FIXER_ALL} ${LibChaosUtils_ExtensionFixer_SOURCES})
TARGET_LINK_LIBRARIES(extensionfixer chaos)

# Audiobook Sorter
IF(NOT LIBCHAOS_UTIL_AUDIOBOOK_SORTER)
    SET(LIBCHAOS_UTIL_AUDIOBOOK_SORTER_ALL EXCLUDE_FROM_ALL)
ENDIF()
ADD_EXECUTABLE(audiobooksorter ${LIBCHAOS_UTIL_AUDIOBOOK_SORTER_ALL} ${LibChaosUtils_AudiobookSorter_SOURCES})
TARGET_LINK_LIBRARIES(audiobooksorter chaos)

# Duplicate Finder
IF(NOT LIBCHAOS_UTIL_DUPLICATE_FINDER)
    SET(LIBCHAOS_UTIL_DUPLICATE_FINDER_ALL EXCLUDE_FROM_ALL)
ENDIF()
ADD_EXECUTABLE(duplicatefinder ${LIBCHAOS_UTIL_DUPLICATE_FINDER_ALL} ${LibChaosUtils_DuplicateFinder_SOURCES})
TARGET_LINK_LIBRARIES(duplicatefinder chaos)

# Image Downloader
IF(NOT LIBCHAOS_UTIL_IMAGE_DOWNLOADER)
    SET(LIBCHAOS_UTIL_IMAGE_DOWNLOADER_ALL EXCLUDE_FROM_ALL)
ENDIF()
ADD_EXECUTABLE(imagedownloader ${LIBCHAOS_UTIL_IMAGE_DOWNLOADER_ALL} ${LibChaosUtils_ImageDownloader_SOURCES})
TARGET_LINK_LIBRARIES(imagedownloader chaos)

# Image Library Manager
IF(NOT LIBCHAOS_UTIL_IMAGE_LIBRARY_MANAGER)
    SET(LIBCHAOS_UTIL_IMAGE_LIBRARY_MANAGER_ALL EXCLUDE_FROM_ALL)
ENDIF()
ADD_EXECUTABLE(imagelibrarymanger ${LIBCHAOS_UTIL_IMAGE_LIBRARY_MANAGER_ALL} ${LibChaosUtils_ImageLibraryManager_SOURCES})
TARGET_LINK_LIBRARIES(imagelibrarymanger chaos)

# Image Reverse Search
IF(NOT LIBCHAOS_UTIL_IMAGE_REVERSE_SEARCH)
    SET(LIBCHAOS_UTIL_IMAGE_REVERSE_SEARCH_ALL EXCLUDE_FROM_ALL)
ENDIF()
ADD_DEFINITIONS(-DCURL_STATICLIB)
ADD_EXECUTABLE(imagereversesearch ${LIBCHAOS_UTIL_IMAGE_REVERSE_SEARCH_ALL} ${LibChaosUtils_ImageReverseSearch_SOURCES})
#TARGET_LINK_LIBRARIES(imagereversesearch chaos ssl ssh2 ssh2dll crypto z rtmp curl curldll)
TARGET_LINK_LIBRARIES(imagereversesearch chaos crypto ssl z curl)
#SET_TARGET_PROPERTIES(imagereversesearch PROPERTIES LINK_FLAGS -Wl,-static)
