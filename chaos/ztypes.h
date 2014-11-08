/*****************************************
**               LibChaos               **
**               ztypes.h               **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZTYPES_H
#define ZTYPES_H

// General LibChaos Notes
// Z-prefixed classes are classes that should be used in user code
// Y-prefixed classes enforce interfaces on inheriting classes
// Other classes from LibChaos are not intended to be used outside the library code

// Require C++
#ifndef __cplusplus
    #error LibChaos requires a C++ compiler!
#endif

// Warn if greater than C++11
#if __cplusplus > 201103L
    #warning LibChaos is not tested with C++ spec newer than C++11
#endif

//
// Compiler
//
#define GCC     0x11
#define MINGW   0x12
#define MSVC    0x13

// Detected
#ifdef __GNUC__
    #ifdef __MINGW32__
        #define COMPILER_DETECTED MINGW
    #else
        #define COMPILER_DETECTED GCC
    #endif
#elif defined(_MSC_VER)
    #define COMPILER_DETECTED MSVC
#else
    #warning Unknown Compiler!
#endif

// Specified
#if defined(_LIBCHAOS_COMPILER_GCC)
    #define COMPILER_SPECIFIED GCC
#elif defined(_LIBCHAOS_COMPILER_MINGW)
    #define COMPILER_SPECIFIED MINGW
#elif defined(_LIBCHAOS_COMPILER_MSVC)
    #define COMPILER_SPECIFIED MSVC
#else
    #warning Unspecified Compiler!
#endif

// Set compiler
#ifdef COMPILER_SPECIFIED
    #define COMPILER COMPILER_SPECIFIED
#else
    #ifdef COMPILER_DETECTED
        #define COMPILER COMPILER_DETECTED
    #else
        #error No detected or specified compiler!
    #endif
#endif

// Warn if detected and specified compilers different
#if COMPILER_DETECTED != COMPILER_SPECIFIED
    #warning Different detected and specified compilers!
#endif

#if COMPILER == GCC || COMPILER == MINGW
    // Require C++11 or greater
    #if __cplusplus < 201103L
        #error LibChaos requires a C++11 compiler!
    #endif
#endif

//
// Platform
//
#define UNIX    0x21
#define LINUX   0x22
#define WINDOWS 0x23
#define MACOS   0x24

// Detected
#if defined(__linux__)
    #define PLATFORM_DETECTED LINUX
#elif defined(_WIN32)
    #define PLATFORM_DETECTED WINDOWS
#elif defined(__APPLE__)
    #define PLATFORM_DETECTED MACOS
#else
    #warning Unknown Platform!
#endif

#if defined(_LIBCHAOS_PLATFORM_LINUX)
    #define PLATFORM_SPECIFIED LINUX
#elif defined(_LIBCHAOS_PLATFORM_WINDOWS)
    #define PLATFORM_SPECIFIED WINDOWS
#else
    #warning Unsupported Platform!
#endif

#ifdef PLATFORM_SPECIFIED
    #define PLATFORM PLATFORM_SPECIFIED
#else
    #ifdef PLATFORM_DETECTED
        #define PLATFORM PLATFORM_DETECTED
    #else
        #error No detected or specified platform!
    #endif
#endif

// Warn if detected and specified platforms different
#if PLATFORM_DETECTED != PLATFORM_SPECIFIED
    #warning Different detected and specified platforms!
#endif

// Only one platform specified
#if defined(_LIBCHAOS_PLATFORM_LINUX) && defined(_LIBCHAOS_PLATFORM_WINDOWS)
    #error Multiple platforms declared. Please declare only one platform at a time.
#endif

// Build
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

// LibChaos experimental versions
#define ZARRAY_VERSION /*1*/ 2
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

// Check sizes
static_assert(sizeof(zs8) == 1, "zs8 has incorrect size");
static_assert(sizeof(zu8) == 1, "zu8 has incorrect size");

static_assert(sizeof(zs16) == 2, "zs16 has incorrect size");
static_assert(sizeof(zu16) == 2, "zu16 has incorrect size");

static_assert(sizeof(zs32) == 4, "zs32 has incorrect size");
static_assert(sizeof(zu32) == 4, "zu32 has incorrect size");

static_assert(sizeof(zs64) == 8, "zs64 has incorrect size");
static_assert(sizeof(zu64) == 8, "zu64 has incorrect size");

}

#endif // ZTYPES_H
