/*******************************************************************************
**                                  LibChaos                                  **
**                                  zerror.h                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZERROR
#define ZERROR

#include "ztypes.h"
#include "zarray.h"
#include "zstring.h"

namespace LibChaos {
namespace ZError {

    enum zerror_signal {
        UNKNOWN = 0,
        INTERRUPT,  // SIGINT
        QUIT,       // SIGQUIT
        ILLEGAL,    // SIGILL
        ABORT,      // SIGABRT
        FPE,        // SIGFPE
        SEGV,       // SIGSEGV
        PIPE,       // SIGPIPE
        ALARM,      // SIGALRM
        TERMINATE,  // SIGTERM
        USR1,       // SIGUSR1
        USR2,       // SIGUSR2
    };

    typedef void (*signalHandler)(zerror_signal);

    struct sigset {
        zerror_signal sigtype;
        signalHandler handler;
    };

    void zassert(bool condition);
    void zassert(bool condition, ZString message);

    void registerSigSegv();
    bool registerInterruptHandler(signalHandler);
    bool registerSignalHandler(zerror_signal, signalHandler);

    #if PLATFORM == WINDOWS
    unsigned long getSystemErrorCode();
    #else
    int getSystemErrorCode();
    #endif

    int getSocketErrorCode();
    ZString getSystemError();

    // TODO: getStackTrace should make an array of TraceFrameInfo structures with more information.
    ArZ getStackTrace(unsigned trim = 1);

    // private
    void sigHandle(int sig);

}

inline void zassert(bool condition){ return ZError::zassert(condition); }
inline void zassert(bool condition, ZString message){ return ZError::zassert(condition, message); }

}

#endif // ZERROR

