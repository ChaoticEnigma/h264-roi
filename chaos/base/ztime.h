/*******************************************************************************
**                                  LibChaos                                  **
**                                  ztime.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZTIME_H
#define ZTIME_H

#include "zstring.h"
#include "zclock.h"

#if COMPILER != MSVC
    #include <ctime>
#endif

namespace LibChaos {

//! Represents an absolute date and time.
class ZTime {
public:
#if COMPILER == MSVC
    typedef zu64 timet;
#else
    typedef time_t timet;
#endif

public:
    ZTime();
    ZTime(const ZTime &other);
    ~ZTime();

    static ZTime now(){
        return ZTime();
    }

    timet getRawTime() const {
        return _time;
    }

    int getSecs() const;

    //! Get a formatted string representation of the time.
    ZString formatStr(ZString format) const;

    ZString timeStr() const;
    ZString dateStr() const;

private:
    timet _time;
};

}

#endif // ZTIME_H
