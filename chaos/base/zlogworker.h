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
#define LOGONLY     "%log%"
#define TIMELOG     "%time% - %log%"
#define TIMETHREAD  "%time% %thread% - %log%"
#define DETAILLOG   "%time% %thread% %function% (%file%:%line%) - %log%"

namespace LibChaos {

//! ZLog entry handler, processor, formatter, writer.
class ZLogWorker {
public:
    struct LogJob {
        int level;

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

    static void logLevelStdOut(int type, ZString fmt);
    static void logLevelStdErr(int level, ZString fmt);
    static void logLevelFile(int level, ZPath file, ZString fmt);

    static void setStdOutEnable(bool set);
    static void setStdErrEnable(bool set);

private:
    static void *zlogWorker(ZThread::ZThreadArg zarg);
    static void sigHandle(int sig);

    static ZString getThread(ztid thread);
    static ZString makeLog(const LogJob *job, ZString fmt);

    ZThread work;
};

} // namespace LibChaos

#endif // ZLOGWORKER_H
