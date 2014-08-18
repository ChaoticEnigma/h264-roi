## LibChaos Utility CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(LibChaos_Utils)

### =================== CONFIG =================== ###

INCLUDE_DIRECTORIES(
    ${LibChaos_Utils_SOURCE_DIR}
    ${LibChaos_INCLUDE}
)

INCLUDE(../configure_build.cmake)
configure_build("Utils" ${LIBCHAOS_BUILD} ${LIBCHAOS_PLATFORM})
SET(LibChaosUtils_BUILD_STRING ${BUILD_STRING} PARENT_SCOPE)

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

# Extension Fixer
ADD_EXECUTABLE(extensionfixer extensionfixer.cpp)
TARGET_LINK_LIBRARIES(extensionfixer chaos)

# Audiobook Sorter
ADD_EXECUTABLE(audiobooksorter audiobooksorter.cpp)
TARGET_LINK_LIBRARIES(audiobooksorter chaos)

# Image Downloader
ADD_EXECUTABLE(imagedownloader imagedownloader.cpp)
TARGET_LINK_LIBRARIES(imagedownloader chaos)


# Duplicate Finder
ADD_EXECUTABLE(duplicatefinder duplicatefinder.cpp)
TARGET_LINK_LIBRARIES(duplicatefinder chaos)
