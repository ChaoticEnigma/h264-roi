## LibChaos UI CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

PROJECT(LibChaos_UI)

### =================== HEADERS =================== ###

SET(LibChaos_UI_HEADERS
    zui.h
    align.h
    base.h
    window.h
    control.h
    button.h
    edit.h
    font.h
    table.h
    progress.h
)

### =================== SOURCES =================== ###

SET(LibChaos_UI_X11_SOURCES
    align.cpp
    button.cpp
    control.cpp
    edit.cpp
    font.cpp
    progress.cpp
    table.cpp
    window.cpp
    windowproc.cpp
)

SET(LibChaos_UI_WIN32_SOURCES
    win32/align_win32.cpp
    win32/button_win32.cpp
    win32/control_win32.cpp
    win32/edit_win32.cpp
    win32/font_win32.cpp
    win32/progress_win32.cpp
    win32/table_win32.cpp
    win32/window_win32.cpp
    win32/windowproc_win32.cpp
)

### =================== CONFIG =================== ###

IF(LIBCHAOS_PLATFORM_WINDOWS)
    SET(LibChaos_UI_SOURCES ${LibChaos_UI_WIN32_SOURCES})
ELSE()
    SET(LibChaos_UI_SOURCES ${LibChaos_UI_X11_SOURCES})
ENDIF()

# Export Properties
GatherSources(${LibChaos_UI_HEADERS} ${LibChaos_UI_X11_SOURCES} ${LibChaos_UI_WIN32_SOURCES})
GatherIncludes(LibChaosUI .)

# Include Directories
GET_PROPERTY(LibChaosInclude GLOBAL PROPERTY LibChaosInclude)
INCLUDE_DIRECTORIES(
    ${LibChaosInclude}
)

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)
IF(LIBCHAOS_PLATFORM_WINDOWS)
    ADD_DEFINITIONS(-DUNICODE -D_UNICODE)
ENDIF()

ADD_LIBRARY(chaosui ${LIBCHAOS_UI_ALL} ${LibChaos_UI_HEADERS} ${LibChaos_UI_SOURCES})
IF(LIBCHAOS_PLATFORM_WINDOWS)
    TARGET_LINK_LIBRARIES(chaosui chaos comctl32)
ELSE()
    TARGET_LINK_LIBRARIES(chaosui chaos)
ENDIF()
