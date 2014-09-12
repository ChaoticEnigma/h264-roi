# libpng CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(libpng)

SET(libpng_HEADERS
    png.h
)

SET(libpng_SOURCES ### =================== SOURCES =================== ###
    png.c
    pngerror.c
    pngget.c
    pngmem.c
    pngpread.c
    pngread.c
    pngrio.c
    pngrtran.c
    pngrutil.c
    pngset.c
    pngtrans.c
    pngwio.c
    pngwrite.c
    pngwtran.c
    pngwutil.c
)

SET(libpng_INCLUDE
    ${libpng_SOURCE_DIR}
    ${zlib_INCLUDE}
    PARENT_SCOPE
)

INCLUDE_DIRECTORIES(
    ${zlib_INCLUDE}
)

SET(${CMAKE_C_FLAGS} "${CMAKE_C_FLAGS} -W -Wall -O2")
ADD_LIBRARY(libpng ${libpng_SOURCES})

