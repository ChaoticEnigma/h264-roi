/*****************************************
**               LibChaos               **
**               zlog.h                 **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZLOG_H
#define ZLOG_H

#include "zlogworker.h"

#define ZLOG_DEBUG_DEPTH 100

#define ZLOG_FILE_PREPROC LibChaos::ZLog::makePreProc(LibChaos::ZLogWorker::file, LibChaos::ZPath(__FILE__).last().str())
#define ZLOG_LINE_PREPROC LibChaos::ZLog::makePreProc(LibChaos::ZLogWorker::line, __LINE__)
#define ZLOG_FUNC_PREPROC LibChaos::ZLog::makePreProc(LibChaos::ZLogWorker::function, __FUNCTION__)

#define PREPROCM ZLOG_FILE_PREPROC << ZLOG_LINE_PREPROC

#if LIBCHAOS_BUILD == LIBCHAOS_RELEASE
    #define LOG(A)  LibChaos::ZLog() << A
    #define DLOG(A)
    #define ELOG(A) LibChaos::ZLog(LibChaos::ZLogSource::error) << PREPROCM << A
    #define RLOG(A) LibChaos::ZLog() << LibChaos::ZLog::raw << A
    #define TLOG(A) LibChaos::ZLog() << LibChaos::ZLog::this_thread << A
    #define OLOG(A) LibChaos::ZLog() << LibChaos::ZLog::stdio << A
#else
    #define LOG(A)  LibChaos::ZLog() << PREPROCM << A
    #define DLOG(A) LibChaos::ZLog(LibChaos::ZLogSource::debug) << PREPROCM << A
    #define ELOG(A) LibChaos::ZLog(LibChaos::ZLogSource::error) << PREPROCM << A
    #define RLOG(A) LibChaos::ZLog() << PREPROCM << LibChaos::ZLog::raw << A
    #define TLOG(A) LibChaos::ZLog() << PREPROCM << LibChaos::ZLog::this_thread << A
    #define OLOG(A) LibChaos::ZLog() << PREPROCM << LibChaos::ZLog::stdio << A
#endif
#define ORLOG(A) OLOG(LibChaos::ZLog::raw << A)

#define IF_LOG(A, B, C, D) if(A){ LOG( B << C ); } else { LOG( B << D ); }
#define IF_DLOG(A, B, C, D) if(A){ DLOG( B << C ); } else { DLOG( B << D ); }

namespace LibChaos {

class ZLog {
public:
    typedef ZLogSource::zlog_source zlog_source;
    typedef ZLogWorker::zlog_preproc zlog_preproc;
    typedef ZLogWorker::info_type info_type;
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
    ~ZLog();

    ZLog &operator<<(zlog_flag);
    ZLog &log(ZString logtext);

    inline ZLog &operator<<(ZString text){ return log(text); }
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

    static zlog_preproc makePreProc(info_type, ZString dat);
    ZLog &operator<<(zlog_preproc info);

    ZString pullBuffer();

    static ZString getDate();
    static ZString getTime();
    static ZString getClock();
    static ZString getThread();
    static ZString genLogFileName(ZString prefix);

    static void init();
    static void init(ZPath, ZString fmt = TIMELOG);

    static void formatStdout(zlog_source type, ZString fmt);
    static void formatStderr(zlog_source type, ZString fmt);
    static void addLogFile(ZPath, zlog_source type, ZString fmt);

    static void waitEnd();
private:
    void flushLog();

    static bool _init;
    static ZLogWorker worker;
    static AsArZ thread_ids;

    ZString buffer;
    ZMap<info_type, ZString> info;
    zlog_source source_mode;
    bool stdiolog;
    bool write_on_destruct;
    bool newline;
    bool rawlog;
    bool noqueue;
};

} // namespace LibChaos

#endif // ZLOG_H
