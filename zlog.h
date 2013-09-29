#ifndef ZLOG_H
#define ZLOG_H

#ifndef BUILDING
#define ZLOG_USE_THREAD
#endif

#include <iostream>
#include <fstream>
#include "zstring.h"
//#include "asar.h"
#include "zthread.h"
#include "zmutex.h"
#include "zpath.h"
//#include <deque>
#include "zlogworker.h"

#define ZLOG_DEBUG_DEPTH 100

#define LOG(A) LibChaos::ZLog() << A
#define DLOG(A) LibChaos::ZLog() << LibChaos::ZLog::debug << A
#define RLOG(A) LibChaos::ZLog() << LibChaos::ZLog::plain_this << A
#define ELOG(A) LibChaos::ZLog() << LibChaos::ZLog::error << A
#define OLOG(A) LibChaos::ZLog() << LibChaos::ZLog::stdout << A

#define LG LibChaos::ZLog() <<
#define DLG LibChaos::ZLog() << LibChaos::ZLog::debug
#define RLG LibChaos::ZLog() << LibChaos::ZLog::plain_this
#define ELG LibChaos::ZLog() << LibChaos::ZLog::error
#define OLG LibChaos::ZLog() << LibChaos::ZLog::stdout

#define IF_LOG(A, B, C, D)  if(A){ LOG( B << C ); } else { LOG( B << D ); }
#define IF_DLOG(A, B, C, D)  if(A){ DLOG( B << C ); } else { DLOG( B << D ); }

namespace LibChaos {

class ZLog {
public:
    struct zlog_flag {
        zlog_flag(short);
        void operator=(short);
        bool operator==(zlog_flag);
        short data;
    };

    static zlog_flag flush; // Manual call to flush log
    static zlog_flag newln; // Append newline to buffer
    static zlog_flag flushln; // Manual call to flush log after appending new newline
    static zlog_flag noln; // Skip next automatic newline

    static zlog_flag normal; // Set instance of class to log normally
    static zlog_flag debug; // Set instance of class to log to debug buffer
    static zlog_flag error; // Current instance to logs and outputs to stderr

    static zlog_flag stdout; // Current instance outputs only to stdout

    ZLog();
    ~ZLog();

    ZLog &operator<<(zlog_flag);
    ZLog &log(ZString logtext);
    ZLog &operator<<(ZString text);
    ZLog &operator<<(std::string text);
    ZLog &operator<<(long int text);
    ZLog &operator<<(const char *text);
    ZLog &operator<<(char *text);
    ZLog &operator<<(ZPath text);
    ZString pullBuffer();

    static ZString getTime();
    static ZString getClock();
    static ZString getThread();
    static ZString genLogFileName(ZString prefix);

    static void init();
    static void init(ZPath);

    static void formatStdout(ZlogFormat, ZlogFormat, ZlogFormat);
    static void formatStderr(ZlogFormat, ZlogFormat, ZlogFormat);
    static void addLogFile(ZPath, ZlogFormat, ZlogFormat, ZlogFormat);
private:
    void flushLog();

    static bool _init;
    static ZLogWorker worker;
    static AsArZ thread_ids;

    bool write_on_destruct;
    ZString buffer;
    bool stdout_this;
    bool newline;
    char source_mode; // normal = 0, debug = 1, error = 2
};

//typedef ZLog::Worker ZLogWorker;

} // namespace LibChaos

#endif // ZLOG_H
