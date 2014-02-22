#ifndef ZTYPES_H
#define ZTYPES_H

#if __cplusplus != 201103L
    #error LibChaos requires a C++11 compiler!
#endif

// Platform detection
#define LINUX   0x01
#define WINDOWS 0x02

#define GCC     0x11
#define MINGW   0x12

#ifdef PLATFORM_LINUX
    #define PLATFORM LINUX
    #define COMPILER GCC
#else
    #ifdef PLATFORM_WINDOWS
        #define PLATFORM WINDOWS
        #define COMPILER MINGW
    #else
        #error Unsupported Platform!
    #endif
#endif

#ifdef PLATFORM_LINUX
    #ifdef PLATFORM_WINDOWS
        #error Multiple platforms declared. Please declare only one platform at a time.
    #endif
#endif

// Constants
#ifndef NULL
    #define NULL (void *)0
#endif

// Macros
#define FOREACH(A) for(zu64 i = 0; i < A; ++i)

// LibChaos version control
#define ZARRAY_VERSION 1
#define ZASSOC_VERSION 1

#include <stdint.h>

namespace LibChaos {

// At least 8 bits (1 byte)
typedef unsigned char zu8;
typedef signed char zs8;
typedef char zchar;

// At least 16 bits (2 bytes)
typedef unsigned short zu16;
typedef signed short zs16;

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
