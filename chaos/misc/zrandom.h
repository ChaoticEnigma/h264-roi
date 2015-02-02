#ifndef ZRANDOM_H
#define ZRANDOM_H

#include "ztypes.h"
#include "zbinary.h"

#if PLATFORM == WINDOWS
    #include <windows.h>
#endif

namespace LibChaos {

class ZRandom {
public:
    ZRandom();

    ZBinary generate(zu64 size);

private:
#if PLATFORM == WINDOWS
    HCRYPTPROV _cryptprov;
#endif
};

}

#endif // ZRANDOM_H
