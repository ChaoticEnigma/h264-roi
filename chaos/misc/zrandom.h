/*******************************************************************************
**                                  LibChaos                                  **
**                                  zrandom.h                                 **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZRANDOM_H
#define ZRANDOM_H

#include "ztypes.h"
#include "zbinary.h"

#if PLATFORM != WINDOWS
    #include <stdio.h>
#endif

namespace LibChaos {

//! Random number generation provider.
class ZRandom {
public:
    ZRandom();
    ~ZRandom();

    zu64 genzu(zu64 min = 0, zu64 max = ZU64_MAX);
    ZBinary generate(zu64 size);

    //! Generate boolean with probability.
    bool chance(double probability);

private:
#if PLATFORM == WINDOWS
    void *_cryptprov;
#else
    FILE *_devrandom;
#endif
};

}

#endif // ZRANDOM_H
