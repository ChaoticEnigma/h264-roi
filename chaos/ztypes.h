/*****************************************
**               LibChaos               **
**               ztypes.h               **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZTYPES_H
#define ZTYPES_H

// Compiler
#define GCC     0x11
#define MINGW   0x12

#if __cplusplus != 201103L
    #error LibChaos requires a C++11 compiler!
#endif

#ifdef _LIBCHAOS_COMPILER_GCC
    #define COMPILER GCC
#else
    #ifdef _LIBCHAOS_COMPILER_MINGW
        #define COMPILER MINGW
    #else
        #error Unsupported Compiler!
    #endif
#endif

#ifdef PLATFORM_LINUX
    #ifdef PLATFORM_WINDOWS
        #error Multiple compilers declared. Please declare only one compiler at a time.
    #endif
#endif

// Platform
#define LINUX   0x01
#define WINDOWS 0x02

#ifdef _LIBCHAOS_PLATFORM_LINUX
    #define PLATFORM LINUX
#else
    #ifdef _LIBCHAOS_PLATFORM_WINDOWS
        #define PLATFORM WINDOWS
    #else
        #error Unsupported Platform!
    #endif
#endif

#ifdef PLATFORM_LINUX
    #ifdef PLATFORM_WINDOWS
        #error Multiple platforms declared. Please declare only one platform at a time.
    #endif
#endif

// Build
#define LIBCHAOS_DEBUG      0x21
#define LIBCHAOS_RELEASE    0x22
#define LIBCHAOS_NORMAL     0x23

#ifdef _LIBCHAOS_BUILD_DEBUG
    #define LIBCHAOS_BUILD LIBCHAOS_DEBUG
#else
    #ifdef _LIBCHAOS_BUILD_RELEASE
        #define LIBCHAOS_BUILD LIBCHAOS_RELEASE
    #else
        #define LIBCHAOS_BUILD LIBCHAOS_NORMAL
    #endif
#endif

// Constants
#ifdef NULL
    #undef NULL
    //#define NULL (void *)0
    #define NULL nullptr
#endif

// Macros
#define FOREACH(A) for(zu64 i = 0; i < A; ++i)
#define MIN(A, B) ((A < B) ? A : B)
#define MAX(A, B) ((A > B) ? A : B)

// LibChaos version control
#define ZARRAY_VERSION 1 //2
#define ZASSOC_VERSION 1 //2

#include <stdint.h>

namespace LibChaos {

// At least 8 bits (1 byte)
typedef unsigned char zu8;
typedef signed char zs8;
typedef char zchar;
typedef zu8 zbyte;

// At least 16 bits (2 bytes)
typedef unsigned short zu16;
typedef signed short zs16;

typedef int zint;
typedef unsigned int zuint;
typedef signed int zsint;

// At least 32 bits (4 bytes)
typedef unsigned long zu32;
typedef signed long zs32;

// At least 64 bits (8 bytes)
typedef unsigned long long zu64;
typedef signed long long zs64;

// Fixed types
typedef int8_t zs8t;
typedef uint8_t zu8t;

typedef int16_t zs16t;
typedef uint16_t zu16t;

typedef int32_t zs32t;
typedef uint32_t zu32t;

typedef int64_t zs64t;
typedef uint64_t zu64t;

}

#endif // ZTYPES_H
