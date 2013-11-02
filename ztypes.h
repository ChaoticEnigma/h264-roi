#ifndef ZTYPES_H
#define ZTYPES_H

// At least 8 bits (1 byte)
typedef unsigned char zu8;
typedef signed char zs8;

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

// TEMPORARY, ONLY AFFECTS HIGHLIGHTING, NOT BUILD
#ifndef BUILDING
    //#define PLATFORM_WINDOWS
    //#define COMPILER_MINGW
    #define PLATFORM_LINUX
    #define COMPILER_GCC
#endif

#endif // ZTYPES_H
