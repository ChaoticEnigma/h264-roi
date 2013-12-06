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

namespace LibChaos {

struct ZlogFormat {
    ZlogFormat() : _init(false){}
    ZlogFormat(bool en, bool pr, int tm, bool th) : _init(true), enable(en), prefix(pr), time(tm), thread(th){}
    bool _init; // does not need to be changed
    bool enable;
    bool prefix;
    int time;
    bool thread;
};

struct LogJob {
    char source;
    ZString time;
    ZString clock;
    ZString thread;
    ZString log;
    bool stdout_this;
    bool newln;
    bool raw;
};

class ZLogWorker {
public:
    struct zlog_out {
        ZlogFormat normal;
        ZlogFormat debug;
        ZlogFormat error;
        AsArZ format;
    };
    struct zlog_outfile {
        ZPath file;
        ZlogFormat normal;
        ZlogFormat debug;
        ZlogFormat error;
        AsArZ format;
    };
#ifdef ZLOG_STD_MUTEX
    static std::mutex mtx;
    //static std::unique_lock<std::mutex> ulock;
    static std::mutex formatMtx;
#else
    static ZMutex<char> mtx;
    static ZMutex<char> formatMtx;
    //pthread_mutex_t pmtx;
#endif
    static std::queue<LogJob> jobs;

    static std::atomic<bool> pending;
    static zlog_out stdoutlog;
    static zlog_out stderrlog;
    static ZArray<zlog_outfile> logfiles;
    static bool lastcomp;

    ZLogWorker();
    ~ZLogWorker();
    int run();
    static void wait();
    void queue(LogJob);
    static void doLog(LogJob jb);

    static void formatStdout(ZlogFormat, ZlogFormat, ZlogFormat);
    static void formatStderr(ZlogFormat, ZlogFormat, ZlogFormat);
    static void addLogFile(ZPath, ZlogFormat, ZlogFormat, ZlogFormat);
private:
    static void *zlogWorker(void *);
    static void sigHandle(int);
    static ZString makeLog(LogJob jb, ZlogFormat fmt);

    ZThread work;
    //boost::thread work;
    //std::thread work;
    //pthread_t work;
};

} // namespace LibChaos

#endif // ZLOGWORKER_H
