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

    double getSecs(){
        return (double)rawclock / (double)CLOCKS_PER_SEC;
    }

    ZString getClock(){
        float rawsecs = getSecs();
        int secs = rawsecs;
        int msecs = (rawsecs - (float)secs) * 1000;
        int mins = secs / 60;
        secs = secs - (mins * 60);
        int hrs = mins / 60;
        mins = mins - (hrs * 60);
        char buffer[20];
        sprintf(buffer, "%02d:%02d:%02d:%03d", hrs, mins, secs, msecs);
        ZString out(buffer);
        return out;
    }

private:
    clock_t rawclock;
};

}

#endif // ZCLOCK_H
