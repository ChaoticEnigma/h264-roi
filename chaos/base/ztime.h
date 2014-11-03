#ifndef ZTIME_H
#define ZTIME_H

#include <ctime>
#include "zstring.h"
#include "zclock.h"

namespace LibChaos {

class ZTime {
public:
    ZTime(time_t raw){
        _time = raw;
    }

    static ZTime now(){
        return ZTime(time(NULL));
    }

    time_t getRawTime(){
        return _time;
    }

    int getSecs(){
        tm *time;
    }

    ZString getDate(){
        struct tm *time;
        time = localtime(&_time);
        char buffer[20];
        sprintf(buffer, "%02d/%02d/%02d", time->tm_mon + 1, time->tm_mday, time->tm_year - 100);
        ZString out(buffer);
        return out;
    }
    ZString getTime(){
        struct tm *time;
        time = localtime(&_time);
        char buffer[20];
        sprintf(buffer, "%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec);
        ZString out(buffer);
        return out;
    }

private:
    time_t _time;
};

}

#endif // ZTIME_H
