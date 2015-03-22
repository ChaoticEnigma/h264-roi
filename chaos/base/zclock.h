/*******************************************************************************
**                                  LibChaos                                  **
**                                  zclock.h                                  **
**                          (c) 2015 Zennix Studios                           **
*******************************************************************************/
#ifndef ZCLOCK_H
#define ZCLOCK_H

#include <ctime>
#include "zstring.h"

namespace LibChaos {

class ZClock {
public:
    ZClock(){
        rawclock = clock();
    }

    double getSecs() const {
        return (double)rawclock / (double)CLOCKS_PER_SEC;
    }

    ZString getClock() const;

private:
    clock_t rawclock;
};

}

#endif // ZCLOCK_H
