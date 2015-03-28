
FUNCTION(configure_build BUILD)

    #MESSAGE(STATUS "Detected: ${CMAKE_SYSTEM_NAME}-${CMAKE_CXX_COMPILER_ID}")

    SET(COMPILER_GCC        0x11)
    SET(COMPILER_MINGW      0x12)
    SET(COMPILER_MSVC       0x13)
    SET(COMPILER_CLANG      0x14)

    SET(COMPILER_GCC        ${COMPILER_GCC}     PARENT_SCOPE)
    SET(COMPILER_MINGW      ${COMPILER_MINGW}   PARENT_SCOPE)
    SET(COMPILER_MSVC       ${COMPILER_MSVC}    PARENT_SCOPE)
    SET(COMPILER_CLANG      ${COMPILER_CLANG}   PARENT_SCOPE)

    SET(PLATFORM_LINUX      0x21)
    SET(PLATFORM_FREEBSD    0x22)
    SET(PLATFORM_WINDOWS    0x23)
    SET(PLATFORM_MACOSX     0x24)

    SET(PLATFORM_LINUX      ${PLATFORM_LINUX}   PARENT_SCOPE)
    SET(PLATFORM_FREEBSD    ${PLATFORM_FREEBSD} PARENT_SCOPE)
    SET(PLATFORM_WINDOWS    ${PLATFORM_WINDOWS} PARENT_SCOPE)
    SET(PLATFORM_MACOSX     ${PLATFORM_MACOSX}  PARENT_SCOPE)

    IF(CMAKE_SYSTEM_NAME MATCHES "Linux")               ## Linux
        IF(CMAKE_CXX_COMPILER_ID MATCHES "GNU")         # GCC
            SET(BUILD_STRING "Linux-GCC")
            SET(LIBCHAOS_COMPILER ${COMPILER_GCC})
            SET(LIBCHAOS_PLATFORM ${PLATFORM_LINUX})
        ENDIF()
    ELSEIF(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")         ## FreeBSD
        IF(CMAKE_CXX_COMPILER_ID MATCHES "GNU")         # GCC
            SET(BUILD_STRING "FreeBSD-GCC")
            SET(LIBCHAOS_COMPILER ${COMPILER_GCC})
            SET(LIBCHAOS_PLATFORM ${PLATFORM_FREEBSD})
        ENDIF()
    ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")         ## Windows
        IF(CMAKE_CXX_COMPILER_ID MATCHES "GNU")         # MinGW
            SET(BUILD_STRING "Windows-MinGW")
            SET(LIBCHAOS_COMPILER ${COMPILER_MINGW})
            SET(LIBCHAOS_PLATFORM ${PLATFORM_WINDOWS})
        ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")    # MS Visual C++
            SET(BUILD_STRING "Windows-MSVC")
            SET(LIBCHAOS_COMPILER ${COMPILER_MSVC})
            SET(LIBCHAOS_PLATFORM ${PLATFORM_WINDOWS})
        ENDIF()
    ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Darwin")          ## Mac OS X
        IF(CMAKE_CXX_COMPILER_ID MATCHES "GNU")         # GCC
            SET(BUILD_STRING "MacOSX-GCC")
            SET(LIBCHAOS_COMPILER ${COMPILER_GCC})
            SET(LIBCHAOS_PLATFORM ${PLATFORM_MACOSX})
        ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "Clang")   # Clang
            SET(BUILD_STRING "MacOSX-Clang")
            SET(LIBCHAOS_COMPILER ${COMPILER_CLANG})
            SET(LIBCHAOS_PLATFORM ${PLATFORM_MACOSX})
        ENDIF()
    ENDIF()

    IF(NOT BUILD_STRING)
        IF(LIBCHAOS_UNKNOWN_TOOLCHAIN)
            MESSAGE(STATUS "Attempting Unknown Toolchain")
        ELSE()
            MESSAGE(ERROR "Unknown Toolchain, set LIBCHAOS_UNKNOWN_TOOLCHAIN to force compilation")
        ENDIF()
    ENDIF()

    SET(LIBCHAOS_COMPILER ${LIBCHAOS_COMPILER} PARENT_SCOPE)
    SET(LIBCHAOS_PLATFORM ${LIBCHAOS_PLATFORM} PARENT_SCOPE)

    SET(CXXF "${CXXF} -D_LIBCHAOS_COMPILER=${LIBCHAOS_COMPILER}")
    SET(CXXF "${CXXF} -D_LIBCHAOS_PLATFORM=${LIBCHAOS_PLATFORM}")

    # Platform Settings
    IF(${LIBCHAOS_PLATFORM} STREQUAL ${PLATFORM_LINUX} OR ${LIBCHAOS_PLATFORM} STREQUAL ${PLATFORM_MACOSX})
        SET(CXXGNU "${CXXGNU} -rdynamic")
    ENDIF()

    # Compiler Settings
    IF(${LIBCHAOS_COMPILER} STREQUAL ${COMPILER_MINGW})
        SET(CXXGNU "${CXXGNU} -mwin32 -mwindows")
    ENDIF()

    # Set variables for build type
    IF(BUILD MATCHES 1) # Debug
        ADD_DEFINITIONS(-D_LIBCHAOS_BUILD_DEBUG)
        SET(CXXGNU "${CXXGNU} -g")
        SET(CXXVC "${CXXVC} /Zi /MDd")
        SET(BUILD_STRING "${BUILD_STRING} Debug")
        SET(DEBUG TRUE)
    ELSEIF(BUILD MATCHES 2) # Release
        ADD_DEFINITIONS(-D_LIBCHAOS_BUILD_RELEASE)
        SET(CXXGNU "${CXXGNU} -O3")
        SET(CXXVC "${CXXVC} /GL /MD")
        SET(BUILD_STRING "${BUILD_STRING} Release")
        SET(RELEASE TRUE)
    ELSE() # Normal
        ADD_DEFINITIONS(-D_LIBCHAOS_BUILD_NORMAL)
        SET(BUILD_STRING "${BUILD_STRING} Normal")
    ENDIF()

    SET(CONFIGURE_BUILD_STRING ${BUILD_STRING} PARENT_SCOPE)

    IF(${LIBCHAOS_COMPILER} STREQUAL ${COMPILER_GCC} OR ${LIBCHAOS_COMPILER} STREQUAL ${COMPILER_MINGW} OR ${LIBCHAOS_COMPILER} STREQUAL ${COMPILER_CLANG})
        SET(CXXGNU "${CXXGNU} -std=c++11")

        IF(DEBUG) # Enable gratuitous warnings on debug build
            SET(CXXGNU "${CXXGNU} -Wall -Wextra -Wpedantic")
            #SET(CXXGNU "${CXXGNU} -fms-extensions")
            #SET(CXXF "${CXXF} -Wbloody_everything") # Some day...
            SET(CXXGNU "${CXXGNU} -Wcast-align -Wcast-qual -Wsign-conversion -Wsign-promo")
            SET(CXXGNU "${CXXGNU} -Wformat=2 -Winit-self -Wlogical-op")
            SET(CXXGNU "${CXXGNU} -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual")
            SET(CXXGNU "${CXXGNU} -Wredundant-decls -Wstrict-null-sentinel -Wctor-dtor-privacy -Wdisabled-optimization")
            SET(CXXGNU "${CXXGNU} -Wstrict-overflow=5 -Wswitch-default -Wundef")

            SET(CXXGNU "${CXXGNU} -Werror=return-type") # Should be errors
            #SET(CXXF "${CXXF} -Wshadow ") # Some warnings are too verbose to be useful
            #SET(CXXF "${CXXF} -Wmissing-declarations -Wold-style-cast") # Not actually errors
            SET(CXXGNU "${CXXGNU} -Wno-unused-parameter -Wno-unused") # Disabled Warnings
            IF(NOT COMPILER_MINGW)
                SET(CXXGNU "${CXXGNU} -Wno-comment") # Not recognized on MinGW
            ENDIF()
        ENDIF()

        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXGNU}")

    ELSEIF(${LIBCHAOS_COMPILER} STREQUAL ${COMPILER_MSVC})
        #SET(CXXVC "${CXXVC} /MD")

        SET(LINKVC "/ignore:4221 /wd4221") # LNK4221: File does not define any public symbols
        SET(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} ${LINKVC}"     PARENT_SCOPE)
        SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKVC}"  PARENT_SCOPE)
        SET(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${LINKVC}"  PARENT_SCOPE)
        SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LINKVC}"  PARENT_SCOPE)

        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXVC}")
    ENDIF()

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXF}" PARENT_SCOPE)

ENDFUNCTION()
