/*****************************************
**               LibChaos               **
**               zlog.h                 **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZLOG_H
#define ZLOG_H

#include "zlogworker.h"

#define ZLOG_DEBUG_DEPTH 100

#define ZLOG_FILE_PREPROC LibChaos::ZLog::makePreProc(LibChaos::ZLogWorker::file, ZPath(__FILE__).last().str())
#define ZLOG_LINE_PREPROC LibChaos::ZLog::makePreProc(LibChaos::ZLogWorker::line, __LINE__)
#define ZLOG_FUNC_PREPROC LibChaos::ZLog::makePreProc(LibChaos::ZLogWorker::function, __FUNCTION__)

#define PREPROCM ZLOG_FILE_PREPROC << ZLOG_LINE_PREPROC

#if LIBCHAOS_BUILD == LIBCHAOS_RELEASE
    #define LOG(A)  LibChaos::ZLog() << A
    #define DLOG(A) LibChaos::ZLog(LibChaos::ZLogSource::debug) << PREPROCM << A
    #define ELOG(A) LibChaos::ZLog(LibChaos::ZLogSource::error) << PREPROCM << A
    #define RLOG(A) LibChaos::ZLog() << LibChaos::ZLog::raw << A
    #define SLOG(A) LibChaos::ZLog() << LibChaos::ZLog::sync << A
    #define TLOG(A) LibChaos::ZLog() << LibChaos::ZLog::this_thread << A
    #define OLOG(A) LibChaos::ZLog() << LibChaos::ZLog::stdio << A
#else
    #define LOG(A)  LibChaos::ZLog() << PREPROCM << A
    #define DLOG(A) LibChaos::ZLog(LibChaos::ZLogSource::debug) << PREPROCM << A
    #define ELOG(A) LibChaos::ZLog(LibChaos::ZLogSource::error) << PREPROCM << A
    #define RLOG(A) LibChaos::ZLog() << PREPROCM << LibChaos::ZLog::raw << A
    #define SLOG(A) LibChaos::ZLog() << PREPROCM << LibChaos::ZLog::sync << A
    #define TLOG(A) LibChaos::ZLog() << PREPROCM << LibChaos::ZLog::this_thread << A
    #define OLOG(A) LibChaos::ZLog() << PREPROCM << LibChaos::ZLog::stdio << A
#endif
#define ORLOG(A) OLOG(LibChaos::ZLog::raw << A)

#define IF_LOG(A, B, C, D) if(A){ LOG( B << C ); } else { LOG( B << D ); }
#define IF_DLOG(A, B, C, D) if(A){ DLOG( B << C ); } else { DLOG( B << D ); }

namespace LibChaos {

class ZLog {
public:
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
        sync = 10,          // Push log to queue, block until queue is empty
        async = 11,         // Push log to queue, return as soon as possible (default)
        this_thread = 12    // Log immediately from this thread, block until done
    };

    ZLog(char source = ZLogSource::normal);
    ~ZLog();

    ZLog &operator<<(zlog_flag);
    ZLog &log(ZString logtext);
    ZLog &operator<<(ZString text);
    ZLog &operator<<(std::string text);
    ZLog &operator<<(zs64 text);
    ZLog &operator<<(const char *text);
    ZLog &operator<<(const unsigned char *text);
    ZLog &operator<<(ZPath text);
    ZLog &operator<<(ZBinary text);

    static zlog_preproc makePreProc(info_type, ZString dat);
    ZLog &operator<<(zlog_preproc info);

    ZString pullBuffer();

    static ZString getTime();
    static ZString getClock();
    static ZString getThread();
    static ZString genLogFileName(ZString prefix);

    static void init();
    static void init(ZPath);

    static void formatStdout(zu64 type, ZString fmt);
    static void formatStderr(zu64 type, ZString fmt);
    static void addLogFile(ZPath, zu64 type, ZString fmt);
private:
    void flushLog();

    static bool _init;
    static ZLogWorker worker;
    static AsArZ thread_ids;

    ZString buffer;
    ZMap<info_type, ZString> info;
    char source_mode; // normal = 0, debug = 1, error = 2
    bool stdiolog;
    bool write_on_destruct;
    bool newline;
    bool rawlog;
    bool synclog;
    bool noqueue;

};

} // namespace LibChaos

#endif // ZLOG_H
