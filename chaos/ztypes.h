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

// Signed Integer Encoding
#define LIBCHAOS_COMPLEMENT (-1 & 3)
#if LIBCHAOS_COMPLEMENT != 1 && LIBCHAOS_COMPLEMENT != 2 && LIBCHAOS_COMPLEMENT != 3
    #error Unknown signed integer encoding?
#endif

// Constants
//#ifdef NULL
    #undef NULL
//#endif
//#define NULL (void *)0
#define NULL nullptr

// Macros
#define FOREACH(A) for(zu64 i = 0; i < A; ++i)
#define FOREACHIN(A, B, C) zu64 iloopvar##A##C##__LINE__ = 0; for(B C = A.size() ? A[iloopvar##A##C##__LINE__] : B(); iloopvar##A##C##__LINE__ < A.size(); ++iloopvar##A##C##__LINE__, C = A[iloopvar##A##C##__LINE__])
#define MIN(A, B) ((A < B) ? A : B)
#define MAX(A, B) ((A > B) ? A : B)

// LibChaos version control
#define ZARRAY_VERSION 1 //2
#define ZASSOC_VERSION 1 //2

#include <stdint.h>

namespace LibChaos {

// Exactly 8 bits (1 byte)
typedef unsigned char zuc;
typedef signed char zsc;
typedef char zchar;

// At least 16 bits (2 bytes)
typedef unsigned short zus;
typedef signed short zss;

typedef int zint;
typedef unsigned int zuint;
typedef signed int zsint;

// At least 32 bits (4 bytes)
typedef unsigned long zul;
typedef signed long zsl;

// At least 64 bits (8 bytes)
typedef unsigned long long zull;
typedef signed long long zsll;

// Fixed types
typedef int8_t zs8;
typedef uint8_t zu8;

typedef int16_t zs16;
typedef uint16_t zu16;

typedef int32_t zs32;
typedef uint32_t zu32;

typedef int64_t zs64;
typedef uint64_t zu64;

// Aliases
//typedef zsc zs8;
//typedef zuc zu8;
typedef zu8 zbyte;
typedef zbyte zoctet; // I blame the IETF

//typedef zss zs16;
//typedef zus zu16;

//typedef zsl zs32;
//typedef zul zu32;

//typedef zsll zs64;
//typedef zull zu64;

}

#endif // ZTYPES_H
