
FUNCTION(configure_build NAME BUILD PLATFORM)

    SET(BUILD_STRING "${NAME}:")

    IF(BUILD MATCHES 1)
        ADD_DEFINITIONS(-D_LIBCHAOS_BUILD_DEBUG)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
        SET(BUILD_STRING "${BUILD_STRING} Debug")
    ELSEIF(BUILD MATCHES 2)
        ADD_DEFINITIONS(-D_LIBCHAOS_BUILD_RELEASE)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
        SET(BUILD_STRING "${BUILD_STRING} Release")
    ELSE()
        ADD_DEFINITIONS(-D_LIBCHAOS_BUILD_NORMAL)
        SET(BUILD_STRING "${BUILD_STRING} Normal")
    ENDIF()

    IF(PLATFORM MATCHES 1)
        ADD_DEFINITIONS(-D_LIBCHAOS_PLATFORM_LINUX -D_LIBCHAOS_COMPILER_GCC)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")
        SET(BUILD_STRING "${BUILD_STRING}, Unix")
        SET(PLATFORM_UNIX TRUE PARENT_SCOPE)
    ELSEIF(PLATFORM MATCHES 2)
        ADD_DEFINITIONS(-D_LIBCHAOS_PLATFORM_WINDOWS -D_LIBCHAOS_COMPILER_MINGW)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwin32 -mwindows")
        SET(BUILD_STRING "${BUILD_STRING}, Windows")
        SET(PLATFORM_WINDOWS TRUE PARENT_SCOPE)
    ELSE()
        MESSAGE(ERROR "Unknown Platform")
    ENDIF()

    SET(BUILD_STRING "${BUILD_STRING}" PARENT_SCOPE)

    SET(CXXF         "-std=c++11 -Wall -Wextra -Wpedantic ")
    #SET(CXXF "${CXXF} -Wbloody_everything") # Some day...
    SET(CXXF "${CXXF} -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization")
    SET(CXXF "${CXXF} -Wformat=2 -Winit-self -Wlogical-op")
    SET(CXXF "${CXXF} -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual")
    SET(CXXF "${CXXF} -Wredundant-decls  -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel")
    SET(CXXF "${CXXF} -Wstrict-overflow=5 -Wswitch-default -Wundef")

    #SET(CXXF "${CXXF} -Wshadow ") # Some warnings are too verbose to be useful
    #SET(CXXF "${CXXF} -Wmissing-declarations -Wold-style-cast") # Not actually errors
    SET(CXXF "${CXXF} -Wno-unused-parameter -Wno-unused") # Disabled

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXF}" PARENT_SCOPE)

ENDFUNCTION(configure_build)
