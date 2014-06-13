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

# Extension Fixer
ADD_EXECUTABLE(audiobooksorter audiobooksorter.cpp)
TARGET_LINK_LIBRARIES(audiobooksorter chaos)
