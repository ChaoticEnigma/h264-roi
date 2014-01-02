TARGET = LibChaosUI
TEMPLATE = lib
CONFIG += staticlib
QT -= core gui widgets

HEADERS += \
    $$PWD/zui.h \
    $$PWD/base.h \
    $$PWD/window.h \
    $$PWD/control.h \
    $$PWD/button.h \
    $$PWD/edit.h \
    $$PWD/font.h \
    $$PWD/table.h \
    libchaos/ui/progress.h \
    libchaos/ui/align.h

SOURCES += \
    $$PWD/win32/window_win32.cpp \
    $$PWD/win32/windowproc_win32.cpp \
    $$PWD/win32/control_win32.cpp \
    $$PWD/win32/button_win32.cpp \
    $$PWD/win32/edit_win32.cpp \
    $$PWD/win32/font_win32.cpp \
    $$PWD/win32/table_win32.cpp \
    libchaos/ui/win32/progress_win32.cpp \
    libchaos/ui/win32/align_win32.cpp

OTHER_FILES += $$PWD/CMakeLists.txt

INCLUDEPATH += $$PWD

win32:DEFINES += PLATFORM_WINDOWS COMPILER_MINGW
else:DEFINES += PLATFORM_LINUX COMPILER_GCC
