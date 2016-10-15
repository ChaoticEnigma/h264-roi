## Configure Build Options

SET(COMPILER_GCC        0x11)
SET(COMPILER_MINGW      0x12)
SET(COMPILER_MSVC       0x13)
SET(COMPILER_CLANG      0x14)

SET(PLATFORM_LINUX      0x21)
SET(PLATFORM_FREEBSD    0x22)
SET(PLATFORM_WINDOWS    0x23)
SET(PLATFORM_MACOSX     0x24)
SET(PLATFORM_CYGWIN     0x25)

SET(CXXF "-DBUILDING")

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")               ## Linux
    SET(LIBCHAOS_PLATFORM ${PLATFORM_LINUX})
    SET(LIBCHAOS_PLATFORM_NAME "Linux")
    SET(LIBCHAOS_PLATFORM_LINUX TRUE)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")         ## FreeBSD
    SET(LIBCHAOS_PLATFORM ${PLATFORM_FREEBSD})
    SET(LIBCHAOS_PLATFORM_NAME "FreeBSD")
    SET(LIBCHAOS_PLATFORM_FREEBSD TRUE)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")         ## Windows
    SET(LIBCHAOS_PLATFORM ${PLATFORM_WINDOWS})
    SET(LIBCHAOS_PLATFORM_NAME "Windows")
    SET(LIBCHAOS_PLATFORM_WINDOWS TRUE)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "CYGWIN"           ## Cygwin / MSYS
    OR CMAKE_SYSTEM_NAME MATCHES "MSYS")
    SET(LIBCHAOS_PLATFORM ${PLATFORM_CYGWIN})
    SET(LIBCHAOS_PLATFORM_NAME "Cygwin")
    SET(LIBCHAOS_PLATFORM_CYGWIN TRUE)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Darwin")          ## Mac OS X
    SET(LIBCHAOS_PLATFORM ${PLATFORM_MACOSX})
    SET(LIBCHAOS_PLATFORM_NAME "MacOSX")
    SET(LIBCHAOS_PLATFORM_MACOSX TRUE)
ELSE()
    SET(LIBCHAOS_UNKNOWN_TOOLCHAIN TRUE)
ENDIF()

IF(CMAKE_CXX_COMPILER_ID MATCHES "GNU")             ## GNU
    IF(LIBCHAOS_PLATFORM_WINDOWS OR
       LIBCHAOS_PLATFORM_CYGWIN)                    ## MinGW
        SET(LIBCHAOS_COMPILER ${COMPILER_MINGW})
        SET(LIBCHAOS_COMPILER_NAME "MinGW")
        SET(LIBCHAOS_COMPILER_MINGW TRUE)
    ELSE()                                          ## GCC
        SET(LIBCHAOS_COMPILER ${COMPILER_GCC})
        SET(LIBCHAOS_COMPILER_NAME "GNU")
        SET(LIBCHAOS_COMPILER_GCC TRUE)
    ENDIF()
ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "Clang")       ## Clang
    SET(LIBCHAOS_COMPILER ${COMPILER_CLANG})
    SET(LIBCHAOS_COMPILER_NAME "Clang")
    SET(LIBCHAOS_COMPILER_CLANG TRUE)
ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")        ## MS Visual C++
    SET(LIBCHAOS_COMPILER ${COMPILER_MSVC})
    SET(LIBCHAOS_COMPILER_NAME "MSVC")
    SET(LIBCHAOS_COMPILER_MSVC TRUE)
ELSE()
    SET(LIBCHAOS_UNKNOWN_TOOLCHAIN TRUE)
ENDIF()

SET(BUILD_STRING "${LIBCHAOS_PLATFORM_NAME}-${LIBCHAOS_COMPILER_NAME}")

IF(LIBCHAOS_UNKNOWN_TOOLCHAIN)
    MESSAGE(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
    MESSAGE(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
    MESSAGE(STATUS "Tool Path: ${CMAKE_CXX_COMPILER}")
    IF(LIBCHAOS_FORCE_TOOLCHAIN)
        MESSAGE(STATUS "Attempting Unknown Toolchain")
    ELSE()
        MESSAGE(FATAL_ERROR "Unknown Toolchain, set LIBCHAOS_FORCE_TOOLCHAIN to force compilation")
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
IF(LIBCHAOS_COMPILER_GCC AND NOT LIBCHAOS_PLATFORM_CYGWIN)
    SET(CXXGNU "${CXXGNU} -rdynamic")
ENDIF()
IF(LIBCHAOS_COMPILER_MINGW)
    SET(CXXGNU "${CXXGNU} -mwin32 -mwindows -static-libgcc -static-libstdc++")
ENDIF()

# Check CMake build type
IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(DEBUG TRUE)
ELSEIF(CMAKE_BUILD_TYPE MATCHES "Release")
    SET(RELEASE TRUE)
ELSEIF(DEBUG)
    SET(CMAKE_BUILD_TYPE "Debug")
ELSEIF(RELEASE)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

# Set variables for build type
IF(DEBUG) # Debug
    SET(CXXF "${CXXF} -D_LIBCHAOS_BUILD_DEBUG")
    IF(EXTRA_WARNINGS)
        SET(BUILD_STRING "${BUILD_STRING} DebugV")
    ELSE()
        SET(BUILD_STRING "${BUILD_STRING} Debug")
    ENDIF()
    SET(LIBCHAOS_BUILD_DEBUG TRUE)
ELSEIF(RELEASE) # Release
    SET(CXXF "${CXXF} -D_LIBCHAOS_BUILD_RELEASE")
    SET(BUILD_STRING "${BUILD_STRING} Release")
    SET(LIBCHAOS_BUILD_RELEASE TRUE)
ELSE() # Normal
    SET(BUILD_STRING "${BUILD_STRING} Normal")
ENDIF()

SET(CONFIGURE_BUILD_STRING ${BUILD_STRING})

# Assemble compile flags
IF(LIBCHAOS_COMPILER_GCC OR LIBCHAOS_COMPILER_MINGW OR LIBCHAOS_COMPILER_CLANG)
    # Enable gratuitous warnings on debug build
    IF(DEBUG)
        SET(CXXGNU "${CXXGNU} -Wall -Wextra -Wpedantic")
        #SET(CXXGNU "${CXXGNU} -fms-extensions")
        #SET(CXXF "${CXXF} -Wbloody_everything") # Some day...
        #SET(CXXGNU "${CXXGNU} -Wcast-align") # Cast alignment
        SET(CXXGNU "${CXXGNU} -Wcast-qual -Wsign-conversion -Wsign-promo") # Casting
        SET(CXXGNU "${CXXGNU} -Wformat=2 -Wstrict-overflow=5 -Wswitch-default") # Formatting
        SET(CXXGNU "${CXXGNU} -Winit-self  -Wredundant-decls -Wundef -Woverloaded-virtual") # Declarationss
        SET(CXXGNU "${CXXGNU} -Wmissing-include-dirs -Wctor-dtor-privacy -Wdisabled-optimization")

        # GCC only warnings
        IF(NOT LIBCHAOS_COMPILER_CLANG)
            SET(CXXGNU "${CXXGNU} -Wlogical-op -Wnoexcept -Wstrict-null-sentinel")
        ENDIF()

        # Disabled warnings
        SET(CXXF "${CXXF} -Wshadow") # Some warnings are too verbose to be useful
        SET(CXXGNU "${CXXGNU} -Wno-unused-parameter -Wno-unused")
        IF(NOT COMPILER_MINGW)
            SET(CXXGNU "${CXXGNU} -Wno-comment") # Not recognized on MinGW
        ENDIF()

        # Extra warnings
        IF(EXTRA_WARNINGS)
            IF(LIBCHAOS_COMPILER_CLANG)
                SET(CXXGNU "${CXXGNU} -Weverything") # Enable all diagnostics
                SET(CXXGNU "${CXXGNU} -Wno-c++98-compat -Wno-c++98-compat-pedantic") # Disable C++98 compatibility
            ELSE()
                SET(CXXF "${CXXF} -Wmissing-declarations -Wold-style-cast") # Not actually errors
            ENDIF()
        ENDIF()
    ENDIF()

    # Forced errors
    SET(CXXGNU "${CXXGNU} -Werror=return-type") # Should be errors

    # Compiler-specific
    IF(LIBCHAOS_COMPILER_CLANG)
        SET(CXXGNU "${CXXGNU} -Wno-nested-anon-types") # Clang warns about nested anonymous types
    ENDIF()

    SET(CXXF "${CXXF} ${CXXGNU}")

ELSEIF(LIBCHAOS_COMPILER_MSVC)
    SET(LINKVC "/ignore:4221") # LNK4221: File does not define any public symbols
    SET(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} ${LINKVC}")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKVC}")
    SET(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${LINKVC}")
    SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LINKVC}")

    SET(CXXF "${CXXF} ${CXXVC}")
ENDIF()

SET(LIBCHAOS_CXX_FLAGS "${LIBCHAOS_CXX_FLAGS} ${CXXF}")
