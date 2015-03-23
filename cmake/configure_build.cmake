
FUNCTION(configure_build NAME BUILD)

    SET(BUILD_STRING "${NAME}:")

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

    # Set variables for platform type
    IF(APPLE AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # Mac OS X

        ADD_DEFINITIONS(-D_LIBCHAOS_PLATFORM_OSX)

        ADD_DEFINITIONS(-D_LIBCHAOS_COMPILER_GCC)
        SET(CXXGNU "${CXXGNU} -rdynamic")
        SET(BUILD_STRING "${BUILD_STRING}, MacOSX-GCC")
        SET(PLATFORM_MACOSX TRUE PARENT_SCOPE)
        SET(COMPILER_GCC TRUE PARENT_SCOPE)
        SET(GNU_FLAGS TRUE)

    ELSEIF(WIN32) # Windows

        ADD_DEFINITIONS(-D_LIBCHAOS_PLATFORM_WINDOWS)
        SET(PLATFORM_WINDOWS TRUE PARENT_SCOPE)
        IF(MINGW OR USE_MINGW) # MinGW
            ADD_DEFINITIONS(-D_LIBCHAOS_COMPILER_MINGW)
            SET(CXXGNU "${CXXGNU} -mwin32 -mwindows")
            SET(BUILD_STRING "${BUILD_STRING}, Windows-MinGW")
            SET(COMPILER_MINGW TRUE PARENT_SCOPE)
            SET(GNU_FLAGS TRUE)
        ELSEIF(MSVC) # MSVC
            ADD_DEFINITIONS(-D_LIBCHAOS_COMPILER_MSVC)
            SET(BUILD_STRING "${BUILD_STRING}, Windows-MSVC")
            SET(COMPILER_MSVC TRUE PARENT_SCOPE)
            SET(MSVC_FLAGS TRUE)
        ELSE()
            MESSAGE(ERROR "Unknown Compiler")
        ENDIF()

    ELSEIF(UNIX) # UNIX

        ADD_DEFINITIONS(-D_LIBCHAOS_PLATFORM_LINUX -D_LIBCHAOS_COMPILER_GCC)
        SET(CXXGNU "${CXXGNU} -rdynamic")
        SET(BUILD_STRING "${BUILD_STRING}, Unix-GCC")
        SET(PLATFORM_UNIX TRUE PARENT_SCOPE)
        SET(COMPILER_GCC TRUE PARENT_SCOPE)
        SET(GNU_FLAGS TRUE)

    ELSE() # Unknown
        MESSAGE(ERROR "Unknown Platform")
    ENDIF()

    SET(BUILD_STRING "${BUILD_STRING}" PARENT_SCOPE)
    SET(CONFIGURE_BUILD_STRING "${BUILD_STRING}" PARENT_SCOPE)

    IF(GNU_FLAGS)
        SET(CXXGNU "-std=c++11")

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

        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXGNU}" PARENT_SCOPE)
    ENDIF()

    IF(MSVC_FLAGS)
        #SET(CXXVC "${CXXVC} /MD")

        SET(LNKVC "/ignore:4221 /wd4221") # LNK4221: File does not define any public symbols
        SET(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} ${LNKVC}")
        SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LNKVC}")
        SET(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${LNKVC}")
        SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LNKVC}")

        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXVC}" PARENT_SCOPE)
    ENDIF()

ENDFUNCTION(configure_build)
