## Project LibChaosUI CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(LibChaosUI)

#ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../chaos ${CMAKE_CURRENT_BINARY_DIR}/chaos)

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
IF(WINDOWS) ### =================== SOURCES =================== ###
    SET(LibChaosUI_SOURCES
        win32/window_win32.cpp
        win32/windowproc_win32.cpp
        win32/align_win32.cpp
        win32/control_win32.cpp
        win32/button_win32.cpp
        win32/edit_win32.cpp
        win32/font_win32.cpp
        win32/table_win32.cpp
        win32/progress_win32.cpp
    )
ELSE()
    SET(LibChaosUI_SOURCES
        zuiwindow.cpp
        zuiwindowproc.cpp
        zuicontrol.cpp
        zuibutton.cpp
        zuiedit.cpp
        zuifont.cpp
        zuitable.cpp
    )
ENDIF()

### =================== CONFIG =================== ###

SET(LibChaosUI_INCLUDE ${LibChaosUI_SOURCE_DIR} PARENT_SCOPE)

INCLUDE_DIRECTORIES(
    ${LibChaos_INCLUDE}
)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -ansi -std=c++11 -Wl,--subsystem,windows")

### =================== BUILD =================== ###

ADD_DEFINITIONS(-DBUILDING)
ADD_DEFINITIONS(-DUNICODE -D_UNICODE)

ADD_LIBRARY(chaosui ${LibChaosUI_SOURCES})
IF(WINDOWS)
    TARGET_LINK_LIBRARIES(chaosui chaos comctl32)
ELSE()
    TARGET_LINK_LIBRARIES(chaosui chaos)
ENDIF()
