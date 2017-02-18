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
ADD_EXECUTABLE(extensionfixer EXCLUDE_FROM_ALL ${LibChaosUtils_ExtensionFixer_SOURCES})
LibChaos_Configure_Target(extensionfixer)

# Audiobook Sorter
ADD_EXECUTABLE(audiobooksorter EXCLUDE_FROM_ALL ${LibChaosUtils_AudiobookSorter_SOURCES})
LibChaos_Configure_Target(audiobooksorter)

# Duplicate Finder
ADD_EXECUTABLE(duplicatefinder EXCLUDE_FROM_ALL ${LibChaosUtils_DuplicateFinder_SOURCES})
LibChaos_Configure_Target(duplicatefinder)

# Image Library Manager
ADD_EXECUTABLE(imagelibrarymanager EXCLUDE_FROM_ALL ${LibChaosUtils_ImageLibraryManager_SOURCES})
LibChaos_Configure_Target(imagelibrarymanager)

# Image Reverse Search
ADD_DEFINITIONS(-DCURL_STATICLIB)
ADD_EXECUTABLE(imagereversesearch EXCLUDE_FROM_ALL ${LibChaosUtils_ImageReverseSearch_SOURCES})
LibChaos_Configure_Target(imagereversesearch)
TARGET_LINK_LIBRARIES(imagereversesearch crypto ssl z curl)
