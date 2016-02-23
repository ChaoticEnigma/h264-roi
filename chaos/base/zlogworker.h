/*******************************************************************************
**                                  LibChaos                                  **
**                                zlogworker.h                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZLOGWORKER_H
#define ZLOGWORKER_H

#include "zstring.h"
#include "zbinary.h"
#include "zpath.h"

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

//! Asynchronous ZLog log formatter / writer.
class ZLogWorker {
public:
    struct LogJob {
        int source;

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
    void stop();

    void queue(LogJob *job);
    static void doLog(LogJob *job);

    static void formatStdout(int type, ZString fmt);
    static void formatStderr(int type, ZString fmt);
    static void addLogFile(ZPath, int type, ZString fmt);
private:
    static void *zlogWorker(void *);
    static void sigHandle(int);

    static ZString getThread(ztid thread);
    static ZString makeLog(const LogJob *job, ZString fmt);

    ZThread work;
};

} // namespace LibChaos

#endif // ZLOGWORKER_H
