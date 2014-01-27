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

// Default Log formats
#define LOGONLY "%log%"
#define TIMELOG "%time% - %log%"
#define TIMETHREAD "%time% %thread% - %log%"
#define DETAILLOG "%time% %thread% %function% (%file%:%line%) - %log%"

namespace LibChaos {

namespace ZLogSource {
    enum {
        normal = 1,
        debug = 2,
        error = 3
    };
}

class ZLogWorker {
public:
    enum info_type {
        file = 1,
        line = 2,
        function = 3,
        clock = 4,
        time = 5,
        thread = 6
    };

    struct LogJob {
        char source;
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

    static ZMutex<char> mtx;
    static ZMutex<char> formatMtx;
    static std::queue<LogJob> jobs;

    static std::atomic<bool> pending;
    static ZMap<zu64, ZString> stdoutlog;
    static ZMap<zu64, ZString> stderrlog;
    static ZAssoc< ZPath, ZMap<zu64, ZString> > logfiles;
    static bool lastcomp;

    ZLogWorker();
    ~ZLogWorker();
    int run();
    static void wait();
    void queue(LogJob);
    static void doLog(LogJob jb);

    static void formatStdout(zu64 type, ZString fmt);
    static void formatStderr(zu64 type, ZString fmt);
    static void addLogFile(ZPath, zu64 type, ZString fmt);
private:
    static void *zlogWorker(void *);
    static void sigHandle(int);
    static ZString makeLog(LogJob &job, ZString fmt);

    ZThread work;
};

} // namespace LibChaos

#endif // ZLOGWORKER_H
