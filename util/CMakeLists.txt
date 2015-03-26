## LibChaos Utility CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(LibChaos_Utils)

### =================== CONFIG =================== ###

OPTION(LIBCHAOS_EXTERN_EXTENSION_FIXER          "Build LibChaos Extension Fixer"        OFF )
OPTION(LIBCHAOS_EXTERN_AUDIOBOOK_SORTER         "Build LibChaos Audiobook Sorter"       OFF )
OPTION(LIBCHAOS_EXTERN_IMAGE_DOWNLOADER         "Build LibChaos Image Downloader"       ON  )
OPTION(LIBCHAOS_EXTERN_DUPLICATE_FINDER         "Build LibChaos Duplicate Finder"       OFF )
OPTION(LIBCHAOS_EXTERN_IMAGE_LIBRARY_MANAGER    "Build LibChaos Image Library Manager"  ON  )

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
