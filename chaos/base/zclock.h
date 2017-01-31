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

    ZClock(const ZClock &other);

    //! Start clock.
    void start();
    //! Stop clock.
    void stop();

    bool running() const { return run; }


    /*! Get clock in seconds.
     *  If clock is running, get time since clock started.
     *  If clock is not running, get time from when clock was started to when clock was stopped.
     */
    double getSecs() const;

    //! Get clock string.
    ZString str() const;

    //! Return the clock start time.
    timespec getClockStart() const { return clock_a; }
    //! Return the clock stop time.
    timespec getClockStop() const { return clock_b; }

    //! Get time string from clock value.
    static ZString clockStr(timespec clock);

    static ZString diffStr(timespec a, timespec b);

private:
    //! Diff from start to stop.
    timespec diff() const;

    //! Return current clock time.
    timespec _getTime() const;

private:
    bool run;
    timespec clock_a;
    timespec clock_b;
};

}

#endif // ZCLOCK_H
