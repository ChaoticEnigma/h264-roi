/*******************************************************************************
**                                  LibChaos                                  **
**                                   zlog.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZLOG_H
#define ZLOG_H

#include "zlogworker.h"
#include "zexception.h"
#include <atomic>

#define ZLOG_DEBUG_DEPTH 100

#define ZLOG_PREFILE LibChaos::ZPath(__FILE__).last()
#define ZLOG_PRELINE LibChaos::ZString(__LINE__)
#define ZLOG_PREFUNC LibChaos::ZString(__FUNCTION__)

#if LIBCHAOS_BUILD == LIBCHAOS_RELEASE
    #define LOG(A)  LibChaos::ZLog(LibChaos::ZLog::INFO) << A
    #define DLOG(A) LibChaos::ZLog(LibChaos::ZLog::DEBUG) << A
    #define ELOG(A) LibChaos::ZLog(LibChaos::ZLog::ERRORS) << A
    #define RLOG(A) LibChaos::ZLog(LibChaos::ZLog::INFO) << LibChaos::ZLog::RAW << A
    #define TLOG(A) LibChaos::ZLog(LibChaos::ZLog::INFO) << LibChaos::ZLog::THIS_THREAD << A
    #define OLOG(A) LibChaos::ZLog(LibChaos::ZLog::INFO) << LibChaos::ZLog::STDIO << A
#else
    #define LOG(A)  LibChaos::ZLog(LibChaos::ZLog::INFO, ZLOG_PREFILE, ZLOG_PRELINE, ZLOG_PREFUNC) << A
    #define DLOG(A) LibChaos::ZLog(LibChaos::ZLog::DEBUG, ZLOG_PREFILE, ZLOG_PRELINE, ZLOG_PREFUNC) << A
    #define ELOG(A) LibChaos::ZLog(LibChaos::ZLog::ERRORS, ZLOG_PREFILE, ZLOG_PRELINE, ZLOG_PREFUNC) << A
    #define RLOG(A) LibChaos::ZLog(LibChaos::ZLog::INFO, ZLOG_PREFILE, ZLOG_PRELINE, ZLOG_PREFUNC) << LibChaos::ZLog::RAW << A
    #define TLOG(A) LibChaos::ZLog(LibChaos::ZLog::INFO, ZLOG_PREFILE, ZLOG_PRELINE, ZLOG_PREFUNC) << LibChaos::ZLog::THIS_THREAD << A
    #define OLOG(A) LibChaos::ZLog(LibChaos::ZLog::INFO, ZLOG_PREFILE, ZLOG_PRELINE, ZLOG_PREFUNC) << LibChaos::ZLog::STDIO << A
#endif
#define ORLOG(A) OLOG(LibChaos::ZLog::raw << A)

#define IF_LOG(A, B, C, D) if(A){ LOG( B << C ); } else { LOG( B << D ); }
#define IF_DLOG(A, B, C, D) if(A){ DLOG( B << C ); } else { DLOG( B << D ); }

namespace LibChaos {

struct ZLogInfo {
    enum info_type {
        FILE        = 1,    // File name
        LINE        = 2,    // Line number
        FUNCTION    = 3,    // Function name
        CLOCK       = 4,    // Time since program start (hh:mm:ss.mmm)
        DATE        = 5,    // Current date
        TIME        = 6,    // Current time (hh:mm:ss)
        THREAD      = 7,    // Thread number
    };

    ZLogInfo(info_type logtype, ZString loginfo) : type(logtype), info(loginfo){}

    info_type type;
    ZString info;
};

//! Formats a log entry and queues it with a ZLogWorker.
class ZLog {
public:
    typedef ZLogWorker::LogJob LogJob;
    typedef int zlog_level;

    // Log level
    enum {
        ERRORS   = 1,
        WARNING = 2,
        INFO    = 3,
        VERBOSE = 4,
        DEBUG   = 5,
    };

    enum zlog_flag {
        // Actions
        FLUSH       = 1,    // Flush log immediately
        NEWLN       = 2,    // Append newline to buffer
        FLUSHLN     = 3,    // Append newline and flush log

        // Format Modifers
        NOLN        = 4,    // Disable automatic newlines for this object
        RAW         = 5,    // This object will log without formatting

        // Target Modifiers
        STDIO       = 9,    // Current instance outputs only to stdout

        // Sequence Modifiers
        THIS_THREAD = 12,   // Log immediately from this thread, block until done
    };

    ZLog(zlog_level source = ZLog::INFO);
    ZLog(zlog_level source, ZString prefile, ZString preline, ZString prefunc);

    ~ZLog();

    ZLog &operator<<(zlog_flag);
    ZLog &log(ZString logtext);

    ZLog &operator<<(ZLogInfo info);

    inline ZLog &operator<<(ZString text){ return log(text); } // Base overload
    inline ZLog &operator<<(const char *text){ return log(text); }
    inline ZLog &operator<<(ZPath text){ return log(text.str()); }
    inline ZLog &operator<<(ZException e){ return log(ZString(e.code()) + ": " + e.what()); }
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
    inline ZLog &operator<<(double num){ return log(ZString(num)); }
    inline ZLog &operator<<(bool tf){ return log(tf ? "true" : "false"); }

    //! Concat \a text to buffer with prepended space.
    inline ZLog &operator,(ZString text){ return log(" ").log(text); } // Base overload

    //! Get log object buffer and clear object.
    ZString pullBuffer();

    static ZString getThread();
    static ZString genLogFileName(ZString prefix);

    static void init();
    static void deInit();

    //! Set the standard output format string for \a level.
    static inline void logLevelStdOut(zlog_level level, ZString fmt){ ZLogWorker::logLevelStdOut(level, fmt); }
    //! Set the standard error format string for \a level.
    static inline void logLevelStdErr(zlog_level level, ZString fmt){ ZLogWorker::logLevelStdErr(level, fmt); }
    //! Set the file output format string for \a level on \a file.
    static inline void logLevelFile(zlog_level level, ZPath file, ZString fmt){ ZLogWorker::logLevelFile(level, file, fmt); }

private:
    void flushLog(bool final);

    static std::atomic<bool> _init;
    static ZLogWorker *worker;

    LogJob *job;
    bool stdiolog;
    bool newline;
    bool rawlog;
    bool noqueue;
};

} // namespace LibChaos

#endif // ZLOG_H
