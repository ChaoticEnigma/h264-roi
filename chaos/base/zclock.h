/*******************************************************************************
**                                  LibChaos                                  **
**                                  zclock.h                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZCLOCK_H
#define ZCLOCK_H

#include <ctime>
#include "zstring.h"

namespace LibChaos {

//! Measure relative time differences.
class ZClock {
public:
    ZClock(){
        rawclock = clock();
    }

    //! Get clock in seconds.
    double getSecs() const {
        return (double)rawclock / (double)CLOCKS_PER_SEC;
    }

    //! Get clock string.
    ZString str() const;

private:
    clock_t rawclock;
};

}

#endif // ZCLOCK_H
