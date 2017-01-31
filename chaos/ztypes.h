/*******************************************************************************
**                                  LibChaos                                  **
**                                  ztypes.h                                  **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZTYPES_H
#define ZTYPES_H

/*! \file ztypes.h
 *  ztypes header file.
 */

/*! \defgroup String LibChaos Strings
 *  String processing classes.
 *  \defgroup Thread LibChaos Threads
 *  Thread management classes.
 *  \defgroup Network LibChaos Networking
 *  Network and socket abstraction classes.
 */

// Git Describe
#define LIBCHAOS_DESCRIBE _LIBCHAOS_DESCRIBE

// Require C++11
#if __cplusplus < 201103L
    #error LibChaos requires a C++11 compiler!
#endif

// Warn if greater than C++14
#if __cplusplus > 201402L
    #warning LibChaos is not tested with C++ specs after C++14
#endif

//
// Compiler
//

#define GCC         0x11
#define MINGW       0x12
#define MSVC        0x13
#define CLANG       0x14

/*
// Detection
#if defined(__MINGW32__)
    #define COMPILER MINGW
#elif defined(__clang__)
    #define COMPILER CLANG
#elif defined(__GNUC__)
    #define COMPILER GCC
#elif defined(_MSC_VER)
    #define COMPILER MSVC
#else
    #warning Unknown Compiler!
#endif

// Warn if detected and specified compilers are different
#if COMPILER != _LIBCHAOS_COMPILER
    #warning Different detected and specified compilers!
#endif
*/

#define COMPILER _LIBCHAOS_COMPILER

// Disable some MSVC warnings
#if COMPILER == MSVC
    #pragma warning(disable : 4244) // C4244: conversion from 'x' to 'y', possible loss of data
    #pragma warning(disable : 4800) // C4800: forcing value to 'true' or 'false' (performance warning)
    #pragma warning(disable : 4996) // C4996: This function or variable may be unsafe.
    #pragma warning(disable : 4221)
#endif

//
// Platform
//

#define LINUX       0x21
#define FREEBSD     0x22
#define WINDOWS     0x23
#define MACOSX      0x24
#define CYGWIN      0x25

/*
// Detection
#if defined(__linux__)
    #define PLATFORM LINUX
#elif defined(__FreeBSD__)
    #define PLATFORM FREEBSD
#elif defined(_WIN32)
    #define PLATFORM WINDOWS
#elif defined(__APPLE__)
    #define PLATFORM MACOSX
#else
    #warning Unknown Platform!
#endif

// Warn if detected and specified platforms are different
#if PLATFORM != _LIBCHAOS_PLATFORM
    #warning Different detected and specified platforms!
#endif
*/

#define PLATFORM _LIBCHAOS_PLATFORM

//
// Build
//

#define LIBCHAOS_DEBUG      0x31
#define LIBCHAOS_RELEASE    0x32
#define LIBCHAOS_NORMAL     0x33

#if defined(_LIBCHAOS_BUILD_DEBUG)
    #define LIBCHAOS_BUILD LIBCHAOS_DEBUG
#elif defined(_LIBCHAOS_BUILD_RELEASE)
    #define LIBCHAOS_BUILD LIBCHAOS_RELEASE
#else
    #define LIBCHAOS_BUILD LIBCHAOS_NORMAL
#endif

// Signed Integer Encoding
#define LIBCHAOS_COMPLEMENT (-1 & 3)
#if LIBCHAOS_COMPLEMENT != 1 && LIBCHAOS_COMPLEMENT != 2 && LIBCHAOS_COMPLEMENT != 3
    #error Unknown signed integer encoding?
#endif

// Byte Endianess
#define LIBCHAOS_BIG_ENDIAN (*(const zu16 *)"\0\xff" < 0x100) /* pure evil */

// Constants
//#ifdef NULL
//    #undef NULL
//#endif
//#define NULL (void *)0
//#define NULL nullptr

// Macros
#define FOREACH(A) for(zu64 i = 0; i < A; ++i)
#define FOREACHIN(A, B, C) zu64 iloopvar##A##C##__LINE__ = 0; \
    for(B C = A.size() ? A[iloopvar##A##C##__LINE__] : B(); \
        iloopvar##A##C##__LINE__ < A.size(); \
        ++iloopvar##A##C##__LINE__, C = A[iloopvar##A##C##__LINE__]\
    )
#define MIN(A, B) ((A < B) ? A : B)
#define MAX(A, B) ((A > B) ? A : B)
#define ABS(A) (A < 0 ? -A : A)

// LibChaos experimental versions
#define ZARRAY_VERSION /*1*/ 2

#if COMPILER == MSVC
    //#define ZMUTEX_VERSION 2 // CRITICAL_SECTION
    //#define ZMUTEX_VERSION 3 // std::mutex
    #define ZMUTEX_VERSION 4 // Win32 Mutex
#else
    #define ZMUTEX_VERSION 1 // POSIX threads
    //#define ZMUTEX_VERSION 3 // std::mutex
#endif

#include <stdint.h>

namespace LibChaos {

// Exactly 8 bits (1 byte)
typedef unsigned char zuc;
typedef signed char zsc;
typedef char zchar;

// At least 16 bits (2 bytes)
typedef unsigned short int zus;
typedef signed short int zss;

typedef int zint;
typedef unsigned int zuint;
typedef signed int zsint;

// At least 32 bits (4 bytes)
typedef unsigned long int zul;
typedef signed long int zsl;

// At least 64 bits (8 bytes)
typedef unsigned long long int zull;
typedef signed long long int zsll;

// Fixed types
typedef int8_t zs8;
typedef uint8_t zu8;

typedef int16_t zs16;
typedef uint16_t zu16;

typedef int32_t zs32;
typedef uint32_t zu32;

typedef int64_t zs64;
typedef uint64_t zu64;

// Integral type Aliases
//typedef zsc zs8;
//typedef zuc zu8;

//typedef zss zs16;
//typedef zus zu16;

//typedef zsl zs32;
//typedef zul zu32;

//typedef zsll zs64;
//typedef zull zu64;

// Aliases
typedef zu8 zbyte;
typedef zbyte zoctet; // I blame the IETF

typedef zu64 zsize;

#define ZU8_MAX  ((zu8) 0xFFU)
#define ZU16_MAX ((zu16)0xFFFFFU)
#define ZU32_MAX ((zu32)0xFFFFFFFFUL)
#define ZU64_MAX ((zu64)0xFFFFFFFFFFFFFFFFULL)

#define ZS8_MAX  ((zs8) 0x7F)
#define ZS16_MAX ((zs16)0x7FFFF)
#define ZS32_MAX ((zs32)0x7FFFFFFFL)
#define ZS64_MAX ((zs64)0x7FFFFFFFFFFFFFFFLL)

#define ZS8_MIN  (-ZS8_MAX  - 1)
#define ZS16_MIN (-ZS16_MAX - 1)
#define ZS32_MIN (-ZS32_MAX - 1)
#define ZS64_MIN (-ZS64_MAX - 1)

// Check sizes
static_assert(sizeof(zs8) == 1, "zs8 has incorrect size");
static_assert(sizeof(zu8) == 1, "zu8 has incorrect size");

static_assert(sizeof(zs16) == 2, "zs16 has incorrect size");
static_assert(sizeof(zu16) == 2, "zu16 has incorrect size");

static_assert(sizeof(zs32) == 4, "zs32 has incorrect size");
static_assert(sizeof(zu32) == 4, "zu32 has incorrect size");

static_assert(sizeof(zs64) == 8, "zs64 has incorrect size");
static_assert(sizeof(zu64) == 8, "zu64 has incorrect size");

// Force 64-bit platform
#ifndef ALLOW_X32
static_assert(sizeof(void *) == 8, "void pointer is not 64-bit");
#endif

//! Simple exception structure.
struct zexception {
    zexception(const char *err) : what(err){}
    const char *what;
};

//! Get a string describing this version of LibChaos.
static const char *LibChaosDescribe(){
    return LIBCHAOS_DESCRIBE;
}

} // namespace LibChaos

#endif // ZTYPES_H
