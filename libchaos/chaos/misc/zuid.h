#ifndef ZUID_H
#define ZUID_H

#include "ztypes.h"
#include "zstring.h"

namespace LibChaos {

class ZUID {
public:
    ZUID();

    ZString str();

private:
    struct ID {
        zu64 first;
        zu64 second;
    } _id;
};

}

#endif // ZUID_H
