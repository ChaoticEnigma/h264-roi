/*******************************************************************************
**                                  LibChaos                                  **
**                                 zclock.cpp                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zclock.h"
#include <time.h>

#if PLATFORM == MACOSX

#include <mach/mach_time.h>
#define ORWL_NANO (+1.0E-9)
#define ORWL_GIGA UINT64_C(1000000000)

static double orwl_timebase = 0.0;
static uint64_t orwl_timestart = 0;

// http://stackoverflow.com/questions/5167269/clock-gettime-alternative-in-mac-os-x
timespec orwl_gettime(){
    // be more careful in a multithreaded environement
    if(!orwl_timestart){
        mach_timebase_info_data_t tb = { 0, 0 };
        mach_timebase_info(&tb);
        orwl_timebase = tb.numer;
        orwl_timebase /= tb.denom;
        orwl_timestart = mach_absolute_time();
    }
    timespec t;
    double diff = (mach_absolute_time() - orwl_timestart) * orwl_timebase;
    t.tv_sec = diff * ORWL_NANO;
    t.tv_nsec = diff - ((unsigned long long)t.tv_sec * ORWL_GIGA);
    return t;
}

#endif

timespec ts_diff(timespec start, timespec end){
    timespec temp;
    if((end.tv_nsec-start.tv_nsec) < 0){
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

namespace LibChaos {

ZClock::ZClock(){
    start();
}

void ZClock::start(){
    run = true;
#if PLATFORM == MACOSX
    clock_a = orwl_gettime();
#else
    clock_gettime(CLOCK_MONOTONIC, &clock_a);
#endif
}

void ZClock::stop(){
    run = false;
#if PLATFORM == MACOSX
    clock_b = orwl_gettime();
#else
    clock_gettime(CLOCK_MONOTONIC, &clock_b);
#endif
}

timespec ZClock::diff() const {
    timespec c = clock_b;
    if(run){
#if PLATFORM == MACOSX
        c = orwl_gettime();
#else
        clock_gettime(CLOCK_MONOTONIC, &c);
#endif
    }
    return ts_diff(clock_a, c);
}

double ZClock::getSecs() const {
    timespec d = diff();
    return (double)d.tv_sec + ((double)d.tv_nsec / 1000000000);
}

ZString ZClock::str() const {
    return clockStr(diff());
}

ZString ZClock::clockStr(timespec cl){
    double rawsecs = (double)cl.tv_sec + ((double)cl.tv_nsec / 1000000000);
    int secs = rawsecs;
    int msecs = (rawsecs - (double)secs) * 1000;
    int mins = secs / 60;
    secs = secs - (mins * 60);
    int hrs = mins / 60;
    mins = mins - (hrs * 60);
    return ZString::ItoS((zu64)hrs, 10, 2) + ":" + ZString::ItoS((zu64)mins, 10, 2) + ":" + ZString::ItoS((zu64)secs, 10, 2) + ":" + ZString::ItoS((zu64)msecs, 10, 3);
}

}
