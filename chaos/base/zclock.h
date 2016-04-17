/*******************************************************************************
**                                  LibChaos                                  **
**                                  zclock.h                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZCLOCK_H
#define ZCLOCK_H

#include "zstring.h"

namespace LibChaos {

//! Measure relative time differences.
class ZClock {
public:
    //! Create clock, started by default.
    ZClock();

    //! Start clock.
    void start();
    //! Stop clock.
    void stop();

    //! Get clock in seconds.
    double getSecs() const;

    //! Get clock string.
    ZString str() const;

    //! Return the clock start time.
    clock_t getClockStart() const { return clock_a; }

    //! Get time string from clock value.
    static ZString clockStr(clock_t clock);

private:
    bool run;
    clock_t clock_a;
    clock_t clock_b;
};

}

#endif // ZCLOCK_H
