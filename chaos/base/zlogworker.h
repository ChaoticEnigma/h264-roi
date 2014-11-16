/*****************************************
**               LibChaos               **
**               zlogworker.h           **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZLOGWORKER_H
#define ZLOGWORKER_H

#include "zstring.h"
#include "zbinary.h"
#include "zpath.h"

#include "zmap.h"

#include "zthread.h"
#include "zmutex.h"
#include "zcondition.h"

#include "ztime.h"
#include "zclock.h"

// Default Log formats
#define LOGONLY "%log%"
#define TIMELOG "%time% - %log%"
#define TIMETHREAD "%time% %thread% - %log%"
#define DETAILLOG "%time% %thread% %function% (%file%:%line%) - %log%"

namespace LibChaos {

namespace ZLogSource {
    enum zlog_source {
        normal = 1,
        debug = 2,
        error = 3,
        all = 4
    };
}

class ZLogWorker {
public:
    typedef ZLogSource::zlog_source zlog_source;

    struct LogJob {
        zlog_source source;

        ZTime time;
        ZClock clock;
        ztid thread;

        ZString file;
        ZString line;
        ZString func;

        bool stdio;
        bool newln;
        bool raw;
        ZString log;
    };

    static bool lastcomp;

    ZLogWorker();
    ~ZLogWorker();
    void run();
    void waitEnd();
    void queue(LogJob *job);
    static void doLog(LogJob *job);

    static void formatStdout(zlog_source type, ZString fmt);
    static void formatStderr(zlog_source type, ZString fmt);
    static void addLogFile(ZPath, zlog_source type, ZString fmt);
private:
    static void *zlogWorker(void *);
    static void sigHandle(int);
    static ZString makeLog(LogJob *job, ZString fmt);

    ZThread work;
};

} // namespace LibChaos

#endif // ZLOGWORKER_H
