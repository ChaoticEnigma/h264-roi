# zlib CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(zlib)

SET(zlib_HEADERS

)

SET(zlib_SOURCES ### =================== SOURCES =================== ###
    adler32.c
    compress.c
    crc32.c
    deflate.c
    gzclose.c
    gzlib.c
    gzread.c
    gzwrite.c
    infback.c
    inffast.c
    inflate.c
    inftrees.c
    trees.c
    uncompr.c
    zutil.c
)

SET(zlib_RESOURCE
    win32/zlib1.rc
)

SET(zlib_INCLUDE ${zlib_SOURCE_DIR} PARENT_SCOPE)

SET(${CMAKE_C_FLAGS} "${CMAKE_C_FLAGS} -O3 -Wall")

IF(LIBCHAOS_WINDOWS)
    SET(zlib_RESOURCE_OUT zlibrc.o)
    ADD_CUSTOM_COMMAND(OUTPUT ${zlib_RESOURCE_OUT}
        WORKING_DIRECTORY ${zlib_BINARY_DIR}
        COMMAND ${CMAKE_RC_COMPILER} --define GCC_WINDRES -o ${zlib_RESOURCE_OUT} "${zlib_SOURCE_DIR}/${zlib_RESOURCE}"
        DEPENDS ${zlib_RESOURCE}
    )
    ADD_LIBRARY(zlib ${zlib_SOURCES} ${zlib_RESOURCE_OUT})
ELSE()
    ADD_LIBRARY(zlib ${zlib_SOURCES})
ENDIF()
