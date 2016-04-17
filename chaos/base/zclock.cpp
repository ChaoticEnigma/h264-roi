/*******************************************************************************
**                                  LibChaos                                  **
**                                 zclock.cpp                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zclock.h"
#include <ctime>

namespace LibChaos {

ZClock::ZClock(){
    start();
}

void ZClock::start(){
    run = true;
    clock_a = clock();
}

void ZClock::stop(){
    run = false;
    clock_b = clock();
}

double ZClock::getSecs() const {
    clock_t b = clock_b;
    if(run)
        b = clock();
    return (double)(b - clock_a) / (double)CLOCKS_PER_SEC;
}

ZString ZClock::str() const {
    return clockStr(clock_b - clock_a);
}

ZString ZClock::clockStr(clock_t clock){
    float rawsecs = (float)clock / (float)CLOCKS_PER_SEC;
    int secs = rawsecs;
    int msecs = (rawsecs - (float)secs) * 1000;
    int mins = secs / 60;
    secs = secs - (mins * 60);
    int hrs = mins / 60;
    mins = mins - (hrs * 60);
    return ZString::ItoS((zu64)hrs, 10, 2) + ":" + ZString::ItoS((zu64)mins, 10, 2) + ":" + ZString::ItoS((zu64)secs, 10, 2) + ":" + ZString::ItoS((zu64)msecs, 10, 3);
}

}
