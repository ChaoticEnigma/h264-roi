/*******************************************************************************
**                                  LibChaos                                  **
**                                   zlog.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZLOG_H
#define ZLOG_H

#include "zlogworker.h"
#include <atomic>

#define ZLOG_DEBUG_DEPTH 100

#define ZLOG_PREFILE LibChaos::ZPath(__FILE__).last()
#define ZLOG_PRELINE LibChaos::ZString(__LINE__)
#define ZLOG_PREFUNC LibChaos::ZString(__FUNCTION__)

#define ZLOG_FILE_PREPROC LibChaos::ZLogInfo(LibChaos::ZLogInfo::file, ZLOG_PREFILE)
#define ZLOG_LINE_PREPROC LibChaos::ZLogInfo(LibChaos::ZLogInfo::line, ZLOG_PRELINE)
#define ZLOG_FUNC_PREPROC LibChaos::ZLogInfo(LibChaos::ZLogInfo::function, ZLOG_PREFUNC)

#define PREPROCR ZLOG_FILE_PREPROC << ZLOG_LINE_PREPROC
//#define PREPROCD ZLOG_FILE_PREPROC << ZLOG_LINE_PREPROC << ZLOG_FUNC_PREPROC

#if LIBCHAOS_BUILD == LIBCHAOS_RELEASE
    #define LOG(A)  LibChaos::ZLog() << A
    #define DLOG(A)
    #define ELOG(A) LibChaos::ZLog(LibChaos::ZLogSource::error) << PREPROCR << A
    #define RLOG(A) LibChaos::ZLog() << LibChaos::ZLog::raw << A
    #define TLOG(A) LibChaos::ZLog() << LibChaos::ZLog::this_thread << A
    #define OLOG(A) LibChaos::ZLog() << LibChaos::ZLog::stdio << A
#else
    #define LOG(A)  LibChaos::ZLog(LibChaos::ZLogSource::normal, ZLOG_PREFILE, ZLOG_PRELINE, ZLOG_PREFUNC) << A
    #define DLOG(A) LibChaos::ZLog(LibChaos::ZLogSource::debug, ZLOG_PREFILE, ZLOG_PRELINE, ZLOG_PREFUNC) << A
    #define ELOG(A) LibChaos::ZLog(LibChaos::ZLogSource::error, ZLOG_PREFILE, ZLOG_PRELINE, ZLOG_PREFUNC) << A
    #define RLOG(A) LibChaos::ZLog(LibChaos::ZLogSource::normal, ZLOG_PREFILE, ZLOG_PRELINE, ZLOG_PREFUNC) << LibChaos::ZLog::raw << A
    #define TLOG(A) LibChaos::ZLog(LibChaos::ZLogSource::normal, ZLOG_PREFILE, ZLOG_PRELINE, ZLOG_PREFUNC) << LibChaos::ZLog::this_thread << A
    #define OLOG(A) LibChaos::ZLog(LibChaos::ZLogSource::normal, ZLOG_PREFILE, ZLOG_PRELINE, ZLOG_PREFUNC) << LibChaos::ZLog::stdio << A
#endif
#define ORLOG(A) OLOG(LibChaos::ZLog::raw << A)

#define IF_LOG(A, B, C, D) if(A){ LOG( B << C ); } else { LOG( B << D ); }
#define IF_DLOG(A, B, C, D) if(A){ DLOG( B << C ); } else { DLOG( B << D ); }

namespace LibChaos {

struct ZLogInfo {
    enum info_type {
        file = 1,
        line = 2,
        function = 3,

        clock = 4,
        date = 5,
        time = 6,
        thread = 7
    };

    ZLogInfo(info_type type, ZString info) : type(type), info(info){}

    info_type type;
    ZString info;
};

class ZLog {
public:
    typedef ZLogSource::zlog_source zlog_source;
    typedef ZLogWorker::LogJob LogJob;

    enum zlog_flag {
        // Actions
        flush = 1,          // Flush log immediately
        newln = 2,          // Append newline to buffer
        flushln = 3,        // Append newline and flush log

        // Format Modifers
        noln = 4,           // Disable automatic newlines for this object
        raw = 5,            // This object will log without formatting

        // Target Modifiers
        stdio = 9,          // Current instance outputs only to stdout

        // Sequence Modifiers
        this_thread = 12    // Log immediately from this thread, block until done
    };

    ZLog(zlog_source source = ZLogSource::normal);
    ZLog(zlog_source source, ZString prefile, ZString preline, ZString prefunc);

    ~ZLog();

    ZLog &operator<<(zlog_flag);
    ZLog &log(ZString logtext);

    ZLog &operator<<(ZLogInfo info);

    inline ZLog &operator<<(ZString text){ return log(text); } // Base overload
    inline ZLog &operator<<(ZPath text){ return log(text.str()); }
    ZLog &operator<<(ZBinary text);

    inline ZLog &operator<<(char text){ return log(ZString(text)); }
    inline ZLog &operator<<(unsigned char text){ return log(ZString(text)); }
    inline ZLog &operator<<(zus num){ return log(ZString(num)); }
    inline ZLog &operator<<(zss num){ return log(ZString(num)); }
    inline ZLog &operator<<(zuint num){ return log(ZString(num)); }
    inline ZLog &operator<<(zint num){ return log(ZString(num)); }
    inline ZLog &operator<<(zul num){ return log(ZString(num)); }
    inline ZLog &operator<<(zsl num){ return log(ZString(num)); }
    inline ZLog &operator<<(zull num){ return log(ZString(num)); }
    inline ZLog &operator<<(zsll num){ return log(ZString(num)); }

    inline ZLog &operator<<(bool tf){ return log(tf ? "true" : "false"); }
    inline ZLog &operator<<(double num){ return log(ZString(num)); }

    inline ZLog &operator<<(const char *text){ return log(text); }

    inline ZLog &operator,(ZString text){ return log(" ").log(text); } // Base overload

    ZString pullBuffer();

    static ZString getThread();
    static ZString genLogFileName(ZString prefix);

    static void init();

    static void formatStdout(zlog_source type, ZString fmt);
    static void formatStderr(zlog_source type, ZString fmt);
    static void addLogFile(ZPath, zlog_source type, ZString fmt);

    static void waitEnd();
private:
    void flushLog(bool final);

    static std::atomic<bool> _init;
    static ZLogWorker *worker;

    LogJob *job;

    ZString buffer;

    bool stdiolog;
    bool newline;
    bool rawlog;
    bool noqueue;
};

} // namespace LibChaos

#endif // ZLOG_H
