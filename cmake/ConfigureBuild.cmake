# configure_build.cmake
# Configure build options

SET(COMPILER_GCC        0x11)
SET(COMPILER_MINGW      0x12)
SET(COMPILER_MSVC       0x13)
SET(COMPILER_CLANG      0x14)

SET(PLATFORM_LINUX      0x21)
SET(PLATFORM_FREEBSD    0x22)
SET(PLATFORM_WINDOWS    0x23)
SET(PLATFORM_MACOSX     0x24)

SET(CXXF "-DBUILDING")

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")               ## Linux
    SET(LIBCHAOS_PLATFORM ${PLATFORM_LINUX})
    SET(LIBCHAOS_PLATFORM_LINUX TRUE)
    IF(CMAKE_CXX_COMPILER_ID MATCHES "GNU")         # GCC
        SET(BUILD_STRING "Linux-GCC")
        SET(LIBCHAOS_COMPILER ${COMPILER_GCC})
        SET(LIBCHAOS_COMPILER_GCC TRUE)
    ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "Clang")   # Clang
        SET(BUILD_STRING "Linux-Clang")
        SET(LIBCHAOS_COMPILER ${COMPILER_CLANG})
        SET(LIBCHAOS_COMPILER_CLANG TRUE)
    ENDIF()
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")         ## FreeBSD
    SET(LIBCHAOS_PLATFORM ${PLATFORM_FREEBSD})
    SET(LIBCHAOS_PLATFORM_FREEBSD TRUE)
    IF(CMAKE_CXX_COMPILER_ID MATCHES "GNU")         # GCC
        SET(BUILD_STRING "FreeBSD-GCC")
        SET(LIBCHAOS_COMPILER ${COMPILER_GCC})
        SET(LIBCHAOS_COMPILER_GCC TRUE)
    ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "Clang")   # Clang
        SET(BUILD_STRING "FreeBSD-Clang")
        SET(LIBCHAOS_COMPILER ${COMPILER_CLANG})
        SET(LIBCHAOS_COMPILER_CLANG TRUE)
    ENDIF()
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")         ## Windows
    SET(LIBCHAOS_PLATFORM ${PLATFORM_WINDOWS})
    SET(LIBCHAOS_PLATFORM_WINDOWS TRUE)
    IF(CMAKE_CXX_COMPILER_ID MATCHES "GNU")         # MinGW
        SET(BUILD_STRING "Windows-MinGW")
        SET(LIBCHAOS_COMPILER ${COMPILER_MINGW})
        SET(LIBCHAOS_COMPILER_MINGW TRUE)
    ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")    # MS Visual C++
        SET(BUILD_STRING "Windows-MSVC")
        SET(LIBCHAOS_COMPILER ${COMPILER_MSVC})
        SET(LIBCHAOS_COMPILER_MSVC TRUE)
    ENDIF()
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Darwin")          ## Mac OS X
    SET(LIBCHAOS_PLATFORM ${PLATFORM_MACOSX})
    SET(LIBCHAOS_PLATFORM_MACOSX TRUE)
    IF(CMAKE_CXX_COMPILER_ID MATCHES "GNU")         # GCC
        SET(BUILD_STRING "MacOSX-GCC")
        SET(LIBCHAOS_COMPILER ${COMPILER_GCC})
        SET(LIBCHAOS_COMPILER_GCC TRUE)
    ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "Clang")   # Clang
        SET(BUILD_STRING "MacOSX-Clang")
        SET(LIBCHAOS_COMPILER ${COMPILER_CLANG})
        SET(LIBCHAOS_COMPILER_CLANG TRUE)
    ENDIF()
ENDIF()

IF(NOT BUILD_STRING)
    IF(LIBCHAOS_UNKNOWN_TOOLCHAIN)
        MESSAGE(STATUS "Attempting Unknown Toolchain")
    ELSE()
        MESSAGE(ERROR "Unknown Toolchain, set LIBCHAOS_UNKNOWN_TOOLCHAIN to force compilation")
    ENDIF()
ENDIF()

SET(CXXF "${CXXF} -D_LIBCHAOS_COMPILER=${LIBCHAOS_COMPILER}")
SET(CXXF "${CXXF} -D_LIBCHAOS_PLATFORM=${LIBCHAOS_PLATFORM}")

# Platform Settings
IF(LIBCHAOS_PLATFORM_LINUX OR LIBCHAOS_PLATFORM_MACOSX)
    IF(NOT LIBCHAOS_COMPILER_CLANG})
        #SET(CXXGNU "${CXXGNU} -rdynamic")
    ENDIF()
ENDIF()

# Compiler Settings
IF(LIBCHAOS_COMPILER_GCC)
    SET(CXXGNU "${CXXGNU} -rdynamic")
ELSEIF(LIBCHAOS_COMPILER_MINGW)
    SET(CXXGNU "${CXXGNU} -mwin32 -mwindows")
ENDIF()

# Set variables for build type
IF(DEBUG) # Debug
    SET(CXXF "${CXXF} -D_LIBCHAOS_BUILD_DEBUG")
    SET(CXXGNU "${CXXGNU} -g")
    SET(CXXVC "${CXXVC} /Zi /MDd")
    SET(BUILD_STRING "${BUILD_STRING} Debug")
    SET(LIBCHAOS_BUILD_DEBUG TRUE)
ELSEIF(RELEASE) # Release
    SET(CXXF "${CXXF} -D_LIBCHAOS_BUILD_RELEASE")
    SET(CXXGNU "${CXXGNU} -O3")
    SET(CXXVC "${CXXVC} /GL /MD")
    SET(BUILD_STRING "${BUILD_STRING} Release")
    SET(LIBCHAOS_BUILD_RELEASE TRUE)
ELSE() # Normal
    SET(BUILD_STRING "${BUILD_STRING} Normal")
ENDIF()

SET(CONFIGURE_BUILD_STRING ${BUILD_STRING})

IF(LIBCHAOS_COMPILER_GCC OR LIBCHAOS_COMPILER_MINGW OR LIBCHAOS_COMPILER_CLANG)
    #SET(CXXGNU "${CXXGNU} -std=c++11")
    SET(CXXGNU "${CXXGNU} -std=c++14")

    IF(DEBUG) # Enable gratuitous warnings on debug build
        SET(CXXGNU "${CXXGNU} -Wall -Wextra -Wpedantic")
        #SET(CXXGNU "${CXXGNU} -fms-extensions")
        #SET(CXXF "${CXXF} -Wbloody_everything") # Some day...
        SET(CXXGNU "${CXXGNU} -Wcast-align -Wcast-qual -Wsign-conversion -Wsign-promo") # Casting
        SET(CXXGNU "${CXXGNU} -Wformat=2 -Wstrict-overflow=5 -Wswitch-default") # Formatting
        SET(CXXGNU "${CXXGNU} -Winit-self  -Wredundant-decls -Wundef -Woverloaded-virtual") # Declarationss
        SET(CXXGNU "${CXXGNU} -Wmissing-include-dirs -Wctor-dtor-privacy -Wdisabled-optimization")

        IF(${LIBCHAOS_COMPILER} STREQUAL ${COMPILER_CLANG})
            # Clang options
        ELSE()
            SET(CXXGNU "${CXXGNU} -Wlogical-op -Wnoexcept -Wstrict-null-sentinel")
        ENDIF()

        SET(CXXGNU "${CXXGNU} -Werror=return-type") # Should be errors
        SET(CXXF "${CXXF} -Wshadow ") # Some warnings are too verbose to be useful
        #SET(CXXF "${CXXF} -Wmissing-declarations -Wold-style-cast") # Not actually errors
        SET(CXXGNU "${CXXGNU} -Wno-unused-parameter -Wno-unused") # Disabled Warnings
        IF(NOT COMPILER_MINGW)
            SET(CXXGNU "${CXXGNU} -Wno-comment") # Not recognized on MinGW
        ENDIF()
    ENDIF()

    IF(LIBCHAOS_COMPILER_CLANG)
        SET(CXXGNU "${CXXGNU} -Wno-nested-anon-types") # Clang warns about nested anonymous types
    ENDIF()

    SET(CXXF "${CXXF} ${CXXGNU}")

ELSEIF(LIBCHAOS_COMPILER_MSVC)
    #SET(CXXVC "${CXXVC} /MD")

    SET(LINKVC "/ignore:4221") # LNK4221: File does not define any public symbols
    SET(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} ${LINKVC}")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKVC}")
    SET(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${LINKVC}")
    SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LINKVC}")

    SET(CXXF "${CXXF} ${CXXVC}")
ENDIF()

SET(LIBCHAOS_CXX_FLAGS "${CXXF}")
