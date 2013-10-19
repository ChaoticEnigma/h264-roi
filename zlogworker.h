#ifndef ZLOGWORKER_H
#define ZLOGWORKER_H

#include <queue>

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
    static ZMutex< std::queue<LogJob> > jobs;
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
    static ZString formatLog(LogJob jb, ZlogFormat fmt);

    ZThread work;
    //boost::thread work;
    //std::thread work;
    //pthread_t work;
};

} // namespace LibChaos

#endif // ZLOGWORKER_H
