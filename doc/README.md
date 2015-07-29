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
        - PNG Images        (\ref LibChaos::ZPNG                "ZPNG")
        - BMP Images        (\ref LibChaos::ZBMP                "ZBMP")
        - PPM Images        (\ref LibChaos::ZPPM                "ZPPM")

- Experimental:
    - Networking            (\ref LibChaos::ZSocket             "ZSocket",
                             \ref LibChaos::ZAddress            "ZAddress",
                             \ref LibChaos::ZDatagramSocket     "ZDatagramSocket",
                             \ref LibChaos::ZStreamSocket       "ZStreamSocket",
                             \ref LibChaos::ZConnection         "ZConnection")
    - H264 Video            (\ref LibChaos::ZH264Encoder        "ZH264Encoder",
                             \ref LibChaos::ZH264Decoder        "ZH264Decoder")

- WIP:
    - Math                  (\ref LibChaos::ZNumber             "ZNumber",
                             \ref LibChaos::ZExpression         "ZExpression",
                             \ref LibChaos::ZFormula            "ZFormula")
    - Configuration Files   (\ref LibChaos::ZOptions            "ZOptions")
    - PDF                   (\ref LibChaos::ZPDF                "ZPDF")
    - XML                   (\ref LibChaos::ZXML                "ZXML")
    - Out-of-Memory Storage (\ref LibChaos::ZDefaultStorage     "ZDefaultStorage",
                             \ref LibChaos::ZFileStorage        "ZFileStorage")

- Planned:
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
