Documentation built for LibChaos ${LIBCHAOS_DESCRIBE}.

## Introduction

LibChaos is a general-purpose C++ utilities library.

### Includes:

- Functional:
    - Data Structures:
        - Array             (\ref LibChaos::ZArray              "ZArray")
        - List              (\ref LibChaos::ZList               "ZList")
        - Stack             (\ref LibChaos::ZStack              "ZStack")
        - Queue             (\ref LibChaos::ZQueue              "ZQueue")
        - Map               (\ref LibChaos::ZMap                "ZMap")
        - Smart Pointer     (\ref LibChaos::ZPointer            "ZPointer")
    - String Manipulation   (\ref LibChaos::ZString             "ZString",
                             \ref LibChaos::ZJSON               "ZJSON")
    - File Manipulation     (\ref LibChaos::ZFile               "ZFile",
                             \ref LibChaos::ZPath               "ZPath")
    - Threading             (\ref LibChaos::ZThread             "ZThread",
                             \ref LibChaos::ZMutex              "ZMutex",
                             \ref LibChaos::ZCondition          "ZCondition",
                             \ref LibChaos::ZLock               "ZLock")
    - Hashing               (\ref LibChaos::ZHash               "ZHash")
    - UUID Generation       (\ref LibChaos::ZUID                "ZUID")
    - Image Format Support:
        - Images            (\ref LibChaos::ZImage              "ZImage")
        - PNG Images        (\ref LibChaos::ZPNG                "ZPNG")
        - BMP Images        (\ref LibChaos::ZBMP                "ZBMP")
        - PPM Images        (\ref LibChaos::ZPPM                "ZPPM")

- Experimental:
    - Networking            (\ref LibChaos::ZSocket             "ZSocket",
                             \ref LibChaos::ZAddress            "ZAddress",
                             \ref LibChaos::ZDatagramSocket     "ZDatagramSocket",
                             \ref LibChaos::ZStreamSocket       "ZStreamSocket",
                             \ref LibChaos::ZConnection         "ZConnection")

- WIP:
    - JPEG Images           (\ref LibChaos::ZJPEG                "ZJPEG")
    - Math                  (\ref LibChaos::ZNumber             "ZNumber",
                             \ref LibChaos::ZExpression         "ZExpression",
                             \ref LibChaos::ZFormula            "ZFormula")
    - Configuration Files   (\ref LibChaos::ZOptions            "ZOptions")
    - PDF                   (\ref LibChaos::ZPDF                "ZPDF")
    - XML                   (\ref LibChaos::ZXML                "ZXML")
    - Out-of-Memory Storage (\ref LibChaos::ZDefaultStorage     "ZDefaultStorage",
                             \ref LibChaos::ZFileStorage        "ZFileStorage")
    - WebP Images           (\ref LibChaos::ZWebP               "ZWebP")

## Building LibChaos

    mkdir LibChaos-build
    cd LibChaos-build
    cmake ../LibChaos
    make


## General LibChaos Notes
Z-prefixed classes are classes that should be used in user code.
Y-prefixed classes enforce interfaces on inheriting classes.
Other classes from LibChaos are not intended to be used outside the library code.

## Exception Policy
Yes, LibChaos makes _some_ use of exceptions. However, exceptions are only generated
on misuse of LibChaos classes and methods, e.g. calling a method on an object in an
invalid state, which should have been tested before the call. LibChaos makes heavy use of
value-return methods (return type is the value the method was called for). This pattern
requires, on error, either a way to leave the method without returing (exception or assert),
or each returned type to be able to indicate invalid state. Simple value types (strings, arrays, etc)
shouldn't need to carry this information. Exceptions are favored over asserts for this purpose,
since they allow the user to capture and get information about the error, e.g. a stack trace.

Methods that can legitimately fail when used correctly (files, networking, etc) will generally
return status codes, and set values through references or pointers (as appropriate).
