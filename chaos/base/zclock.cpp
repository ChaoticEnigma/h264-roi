/*******************************************************************************
**                                  LibChaos                                  **
**                                 zclock.cpp                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zclock.h"

namespace LibChaos {

ZString ZClock::getClock() const {
    float rawsecs = getSecs();
    int secs = rawsecs;
    int msecs = (rawsecs - (float)secs) * 1000;
    int mins = secs / 60;
    secs = secs - (mins * 60);
    int hrs = mins / 60;
    mins = mins - (hrs * 60);
//    char buffer[20];
//    sprintf(buffer, "%02d:%02d:%02d:%03d", hrs, mins, secs, msecs);
//    ZString out(buffer);
    ZString out = ZString::ItoS((zu64)hrs, 10, 2) + ":" + ZString::ItoS((zu64)mins, 10, 2) + ":" + ZString::ItoS((zu64)secs, 10, 2) + ":" + ZString::ItoS((zu64)msecs, 10, 3);
    return out;
}

}
