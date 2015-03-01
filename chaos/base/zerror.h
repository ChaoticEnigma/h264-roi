/*****************************************
**               LibChaos               **
**               zerror.h               **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZERROR_H
#define ZERROR_H

#include "zarray.h"
#include "zstring.h"
//#include "zmap.h"

namespace LibChaos {

class ZError {
public:
    enum zerror_signal {
        unknown = 0,
        interrupt = 1,  // SIGINT
        abort = 2,      // SIGABRT
        quit = 3,       // SIGQUIT
        illegal = 4,    // SIGILL
        segv = 5,       // SIGSEGV
        terminate = 6,  // SIGTERM
        fpe = 7         // SIGFPE
    };

    typedef void (*signalHandler)(zerror_signal);

    struct sigset {
        zerror_signal sigtype;
        signalHandler handler;
    };

public:
    ZError(ZString desc = ZString(), int code = 0, bool trace = true);

    inline const ZString &what() const { return desc; }
    inline int code() const { return err; }

    inline const ArZ &trace() const { return stacktrace; }
    ZString traceStr() const;

    void logStackTrace() const;

public:
    static void assert(bool condition);

    static void registerSigSegv();
    static bool registerInterruptHandler(signalHandler);
    static bool registerSignalHandler(zerror_signal, signalHandler);

#if PLATFORM == WINDOWS
    static unsigned long getSystemErrorCode();
#else
    static int getSystemErrorCode();
#endif
    static int getSocketErrorCode();
    static ZString getSystemError();

    static ArZ getStackTrace(unsigned trim = 1);

private:
    static void sigHandle(int sig);
    //static ZMap<int, sigset> sigmap;

private:
    ZString desc;
    int err;
    ArZ stacktrace;
};

}

#endif // ZERROR_H
