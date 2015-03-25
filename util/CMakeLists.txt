## LibChaos Utility CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(LibChaos_Utils)

### =================== CONFIG =================== ###

OPTION(LIBCHAOS_EXTERN_EXTENSION_FIXER          FALSE "Build LibChaos Extension Fixer")
OPTION(LIBCHAOS_EXTERN_AUDIOBOOK_SORTER         FALSE "Build LibChaos Audiobook Sorter")
OPTION(LIBCHAOS_EXTERN_IMAGE_DOWNLOADER         TRUE  "Build LibChaos Image Downloader")
OPTION(LIBCHAOS_EXTERN_DUPLICATE_FINDER         FALSE "Build LibChaos Duplicate Finder")
OPTION(LIBCHAOS_EXTERN_IMAGE_LIBRARY_MANAGER    TRUE  "Build LibChaos Image Library Manager")

INCLUDE_DIRECTORIES(
    ${LibChaos_Utils_SOURCE_DIR}
    ${LibChaos_INCLUDE}
)

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)

# Extension Fixer
IF(LIBCHAOS_EXTERN_EXTENSION_FIXER)
    ADD_EXECUTABLE(extensionfixer extensionfixer.cpp)
    TARGET_LINK_LIBRARIES(extensionfixer chaos)
ENDIF()

# Audiobook Sorter
IF(LIBCHAOS_EXTERN_AUDIOBOOK_SORTER)
    ADD_EXECUTABLE(audiobooksorter audiobooksorter.cpp)
    TARGET_LINK_LIBRARIES(audiobooksorter chaos)
ENDIF()

# Image Downloader
IF(LIBCHAOS_EXTERN_IMAGE_DOWNLOADER)
    ADD_EXECUTABLE(imagedownloader imagedownloader.cpp)
    TARGET_LINK_LIBRARIES(imagedownloader chaos)
ENDIF()

# Duplicate Finder
IF(LIBCHAOS_EXTERN_DUPLICATE_FINDER)
    ADD_EXECUTABLE(duplicatefinder duplicatefinder.cpp)
    TARGET_LINK_LIBRARIES(duplicatefinder chaos)
ENDIF()

# Image Library Manager
IF(LIBCHAOS_EXTERN_IMAGE_LIBRARY_MANAGER)
    ADD_EXECUTABLE(imagelibrarymanger imagelibrarymanager.cpp)
    TARGET_LINK_LIBRARIES(imagelibrarymanger chaos)
ENDIF()
