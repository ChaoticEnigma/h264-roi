/*******************************************************************************
**                                  LibChaos                                  **
**                                  zclock.h                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZCLOCK_H
#define ZCLOCK_H

#include "zstring.h"
#include <time.h>

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

    bool running() const { return run; }

    timespec diff() const;

    /*! Get clock in seconds.
     *  If clock is running, get time since clock started.
     *  If clock is not running, get time from when clock was started to when clock was stopped.
     */
    double getSecs() const;

    //! Get clock string.
    ZString str() const;

    //! Return the clock start time.
    timespec getClockStart() const { return clock_a; }

    //! Get time string from clock value.
    static ZString clockStr(timespec clock);

private:
    bool run;
    timespec clock_a;
    timespec clock_b;
};

}

#endif // ZCLOCK_H
