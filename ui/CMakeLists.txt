## Project LibChaosUI CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

PROJECT(LibChaosUI)

SET(LibChaosUI_HEADERS ### =================== HEADERS =================== ###S
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

IF(LIBCHAOS_PLATFORM_WINDOWS) ### =================== SOURCES =================== ###
    SET(LibChaosUI_SOURCES
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
ELSE()
    SET(LibChaosUI_SOURCES
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
ENDIF()

### =================== CONFIG =================== ###

SET(LibChaosUI_INCLUDE ${LibChaosUI_SOURCE_DIR} PARENT_SCOPE)

INCLUDE_DIRECTORIES(
    ${LibChaos_INCLUDE}
)

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)
ADD_DEFINITIONS(-DUNICODE -D_UNICODE)

ADD_LIBRARY(chaosui ${LIBCHAOS_UI_ALL} ${LibChaosUI_HEADERS} ${LibChaosUI_SOURCES})
IF(LIBCHAOS_PLATFORM_WINDOWS)
    TARGET_LINK_LIBRARIES(chaosui chaos comctl32)
ELSE()
    TARGET_LINK_LIBRARIES(chaosui chaos)
ENDIF()
