## Utility Functions and Macros

# Configure TARGET with LibChaos compile options
MACRO(LibChaos_Configure_Target TARGET)
    # Require C++11
    SET_PROPERTY(TARGET ${TARGET} PROPERTY CXX_STANDARD_REQUIRED 11)
    # Try to get C++14
    SET_PROPERTY(TARGET ${TARGET} PROPERTY CXX_STANDARD 14)

    # Set compile flags
    GET_PROPERTY(LibChaosCXXFlags GLOBAL PROPERTY LibChaosCXXFlags)
    SET_PROPERTY(TARGET ${TARGET} PROPERTY COMPILE_FLAGS ${LibChaosCXXFlags})

    # Set include directories
    GET_PROPERTY(LibChaosInclude GLOBAL PROPERTY LibChaosInclude)
    TARGET_INCLUDE_DIRECTORIES(${TARGET} PUBLIC ${LibChaosInclude})

    # Link LibChaos
    TARGET_LINK_LIBRARIES(${TARGET} chaos)
ENDMACRO()

# Add each argument to the global source list
FUNCTION(CollectSources)
    GET_PROPERTY(LibChaosAllSources GLOBAL PROPERTY LibChaosAllSources)
    FOREACH(SRC ${ARGV})
        SET(LibChaosAllSources ${LibChaosAllSources} ${CMAKE_CURRENT_SOURCE_DIR}/${SRC})
    ENDFOREACH()
    SET_PROPERTY(GLOBAL PROPERTY LibChaosAllSources ${LibChaosAllSources})
ENDFUNCTION()

# Add each argument to the global include list
FUNCTION(CollectIncludes)
    SET(IncludeProp ${PROJECT_NAME}Include)
    GET_PROPERTY(Include GLOBAL PROPERTY ${IncludeProp})
    FOREACH(DIR ${ARGN})
        SET(Include ${Include} ${CMAKE_CURRENT_SOURCE_DIR}/${DIR})
    ENDFOREACH()
    SET_PROPERTY(GLOBAL PROPERTY ${IncludeProp} ${Include})
ENDFUNCTION()

# Download a file at URL to FILE in build dir
FUNCTION(DownloadFile URL FILE)
    SET(PATH ${CMAKE_CURRENT_BINARY_DIR}/${FILE})
    IF(NOT EXISTS "${PATH}")
        MESSAGE(STATUS "Downloading ${FILE}")
        FILE(DOWNLOAD "${URL}" "${PATH}")
    ENDIF()
ENDFUNCTION()

# Download a tar.* archive at URL and extract to ARCHIVE in build dir
FUNCTION(DownloadArchive URL ARCHIVE)
    SET(PATH ${CMAKE_CURRENT_BINARY_DIR}/${ARCHIVE})
    IF(NOT EXISTS "${PATH}")
        MESSAGE(STATUS "Downloading ${ARCHIVE}")
        FILE(DOWNLOAD "${URL}" "${PATH}")
        GET_FILENAME_COMPONENT(NAME "${ARCHIVE}" NAME_WE)
        EXECUTE_PROCESS(
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${NAME}"
        )
        EXECUTE_PROCESS(
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${NAME}
            COMMAND ${CMAKE_COMMAND} -E tar xzf "${PATH}"
        )
    ENDIF()
ENDFUNCTION()
