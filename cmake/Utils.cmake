
FUNCTION(CollectSources)
    GET_PROPERTY(LibChaosAllSources GLOBAL PROPERTY LibChaosAllSources)
    FOREACH(SRC ${ARGV})
        SET(LibChaosAllSources ${LibChaosAllSources} ${CMAKE_CURRENT_SOURCE_DIR}/${SRC})
    ENDFOREACH()
    SET_PROPERTY(GLOBAL PROPERTY LibChaosAllSources ${LibChaosAllSources})
ENDFUNCTION()

FUNCTION(CollectIncludes)
    SET(IncludeProp ${PROJECT_NAME}Include)
    GET_PROPERTY(Include GLOBAL PROPERTY ${IncludeProp})
    FOREACH(DIR ${ARGN})
        SET(Include ${Include} ${CMAKE_CURRENT_SOURCE_DIR}/${DIR})
    ENDFOREACH()
    SET_PROPERTY(GLOBAL PROPERTY ${IncludeProp} ${Include})
ENDFUNCTION()

FUNCTION(DownloadFile URL FILE)
    SET(PATH ${CMAKE_CURRENT_BINARY_DIR}/${FILE})
    IF(NOT EXISTS "${PATH}")
        MESSAGE(STATUS "Downloading ${FILE}")
        FILE(DOWNLOAD "${URL}" "${PATH}")
    ENDIF()
ENDFUNCTION()

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
