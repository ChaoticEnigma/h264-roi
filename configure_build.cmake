
FUNCTION(configure_build NAME BUILD PLATFORM)

    SET(BUILD_STRING "${NAME}:")

    # Set variables for build type
    IF(BUILD MATCHES 1) # Debug
        ADD_DEFINITIONS(-D_LIBCHAOS_BUILD_DEBUG)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
        SET(BUILD_STRING "${BUILD_STRING} Debug")
    ELSEIF(BUILD MATCHES 2) # Release
        ADD_DEFINITIONS(-D_LIBCHAOS_BUILD_RELEASE)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
        SET(BUILD_STRING "${BUILD_STRING} Release")
    ELSE() # Normal
        ADD_DEFINITIONS(-D_LIBCHAOS_BUILD_NORMAL)
        SET(BUILD_STRING "${BUILD_STRING} Normal")
    ENDIF()

    # Set variables for platform type
    IF(PLATFORM MATCHES 1) # Linux
        ADD_DEFINITIONS(-D_LIBCHAOS_PLATFORM_LINUX -D_LIBCHAOS_COMPILER_GCC)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")
        SET(BUILD_STRING "${BUILD_STRING}, Unix")
        SET(PLATFORM_UNIX TRUE PARENT_SCOPE)
        SET(COMPILER_GCC TRUE)
    ELSEIF(PLATFORM MATCHES 2) # Windows
        ADD_DEFINITIONS(-D_LIBCHAOS_PLATFORM_WINDOWS)
        SET(PLATFORM_WINDOWS TRUE PARENT_SCOPE)
        IF(USE_MINGW) # MinGW
            ADD_DEFINITIONS(-D_LIBCHAOS_COMPILER_MINGW)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwin32 -mwindows")
            SET(BUILD_STRING "${BUILD_STRING}, Windows-MinGW")
            SET(COMPILER_MINGW TRUE)
        ELSE() # MSVC
            ADD_DEFINITIONS(-D_LIBCHAOS_COMPILER_MSVC)
            SET(BUILD_STRING "${BUILD_STRING}, Windows-MSVC")
        ENDIF()
    ELSE() # Unknown
        MESSAGE(ERROR "Unknown Platform")
    ENDIF()

    SET(BUILD_STRING "${BUILD_STRING}" PARENT_SCOPE)

    IF(COMPILER_GCC OR COMPILER_MINGW)
        SET(CXXF         "-std=c++11 -Wall -Wextra -Wpedantic")
        #SET(CXXF "${CXXF} -Wbloody_everything") # Some day...
        SET(CXXF "${CXXF} -Wcast-align -Wcast-qual -Wsign-conversion -Wsign-promo")
        SET(CXXF "${CXXF} -Wformat=2 -Winit-self -Wlogical-op")
        SET(CXXF "${CXXF} -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual")
        SET(CXXF "${CXXF} -Wredundant-decls -Wstrict-null-sentinel -Wctor-dtor-privacy -Wdisabled-optimization")
        SET(CXXF "${CXXF} -Wstrict-overflow=5 -Wswitch-default -Wundef")

        #SET(CXXF "${CXXF} -Wshadow ") # Some warnings are too verbose to be useful
        #SET(CXXF "${CXXF} -Wmissing-declarations -Wold-style-cast") # Not actually errors
        SET(CXXF "${CXXF} -Wno-unused-parameter -Wno-unused") # Disabled Warnings
        IF(NOT PLATFORM MATCHES 2)
            SET(CXXF "${CXXF} -Wno-comment") # Not recognized on MinGW
        ENDIF()

        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXF}" PARENT_SCOPE)
    ENDIF()

ENDFUNCTION(configure_build)
