#ifndef ZLOGWORKER_H
#define ZLOGWORKER_H

#include <queue>
#ifdef ZLOG_STD_MUTEX
    #include <mutex>
#else
    #include <pthread.h>
#endif

#include "zstring.h"
#include "zpath.h"
#include "zthread.h"
#include "zmutex.h"
#include "zbinary.h"
#include "zmap.h"
#include "zcondition.h"

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

    enum info_type {
        file = 1,
        line = 2,
        function = 3,
        clock = 4,
        date = 5,
        time = 6,
        thread = 7
    };

    struct LogJob {
        zlog_source source;
        bool stdio;
        bool newln;
        bool raw;
        ZString log;
        ZMap<info_type, ZString> pinfo;
    };

    struct zlog_preproc {
        info_type type;
        ZString info;
    };

    static ZMutex mtx;
    static ZMutex formatMtx;
    static ZCondition cond;
    static std::queue<LogJob> jobs;

    static ZMap<zlog_source, ZString> stdoutlog;
    static ZMap<zlog_source, ZString> stderrlog;
    static ZAssoc< ZPath, ZMap<zlog_source, ZString> > logfiles;
    static bool lastcomp;

    ZLogWorker();
    ~ZLogWorker();
    void run();
    void waitEnd();
    void queue(LogJob);
    static void doLog(LogJob &jb);

    static void formatStdout(zlog_source type, ZString fmt);
    static void formatStderr(zlog_source type, ZString fmt);
    static void addLogFile(ZPath, zlog_source type, ZString fmt);
private:
    static void *zlogWorker(void *);
    static void sigHandle(int);
    static ZString makeLog(LogJob &job, ZString fmt);

    ZThread work;
};

} // namespace LibChaos

#endif // ZLOGWORKER_H
