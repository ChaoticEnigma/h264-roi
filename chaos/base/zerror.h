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
        unknown = 0,
        interrupt = 1,  // SIGINT
        abort = 2,      // SIGABRT
        quit = 3,       // SIGQUIT
        illegal = 4,    // SIGILL
        segv = 5,       // SIGSEGV
        terminate = 6,  // SIGTERM
        fpe = 7,        // SIGFPE
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

    ArZ getStackTrace(unsigned trim = 1);

    // private
    void sigHandle(int sig);

}

inline void zassert(bool condition){ return ZError::zassert(condition); }
inline void zassert(bool condition, ZString message){ return ZError::zassert(condition, message); }

}

#endif // ZERROR

