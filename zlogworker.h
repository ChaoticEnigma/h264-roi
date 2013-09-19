#ifndef ZLOGWORKER_H
#define ZLOGWORKER_H

#include "zthread.h"
#include "zstring.h"
#include "zmutex.h"
#include "zpath.h"
//#include "zfile.h"
#include <queue>
#include <iostream>
#include <fstream>
//#include <boost/thread.hpp>
//#include <thread>
//#include <pthread.h>

namespace LibChaos {

struct ZlogFormat {
    ZlogFormat() : _init(false){}
    ZlogFormat(bool en, bool pr, int tm, bool th) : _init(true), enable(en), prefix(pr), time(tm), thread(th){}
    bool enable;
    bool prefix;
    int time;
    bool thread;
    bool _init; // does not need to be changed
};

struct LogJob {
    char source;
    ZString time;
    ZString clock;
    ZString thread;
    ZString log;
    bool stdout_this;
    bool newln;
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
    static std::vector<zlog_outfile> logfiles;
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
