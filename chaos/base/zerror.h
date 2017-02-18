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
#include "zpath.h"

//#define ZASSERT(A) ZError::zassert(A)
#define ZASSERT(A, B) ZError::zassert(A, B)

namespace LibChaos {
namespace ZError {

    enum zerror_signal {
        UNKNOWN = 0,
        INTERRUPT,  //!< SIGINT
        QUIT,       //!< SIGQUIT
        ILLEGAL,    //!< SIGILL
        ABORT,      //!< SIGABRT
        FPE,        //!< SIGFPE
        SEGV,       //!< SIGSEGV
        PIPE,       //!< SIGPIPE
        ALARM,      //!< SIGALRM
        TERMINATE,  //!< SIGTERM
        USER1,      //!< SIGUSR1
        USER2,      //!< SIGUSR2
    };

    typedef void (*signalHandler)(zerror_signal);

    struct sigset {
        zerror_signal sigtype;
        signalHandler handler;
    };

    struct TraceFrame {
        unsigned i;             //!< Position in stack.

        ZPath exec;             //!< Executable file.
        zu64 addr;              //!< Address in executable.

        ZPath file;             //!< Source file.
        unsigned line;          //!< Line number.

        ZString symbol;         //!< Mangled symbol name.
        ZString name;           //!< Demangled function name.
        ZString offset;         //!< Offset in function.

    };

    void zassert(bool condition);
    void zassert(bool condition, ZString message);

    void registerSigSegv();
    bool registerInterruptHandler(signalHandler);
    bool registerSignalHandler(zerror_signal, signalHandler);

    //! Get the last error code from the system.
    #if PLATFORM == WINDOWS
    unsigned long getSystemErrorCode();
    #else
    int getSystemErrorCode();
    #endif
    //! Get the last error string from the system.
    ZString getSystemError();

    //! Get the last error code from the socket subsystem.
    int getSocketErrorCode();
    //! Get the last error string from the socket subsystem.
    ZString getSocketError();

    // TODO: getStackTrace should make an array of TraceFrameInfo structures with more information.
    //! Get a stack trace array.
    ZArray<TraceFrame> getStackTrace(unsigned trim = 1);
    ZString traceFrameStr(const TraceFrame &frame);

    // private
    void sigHandle(int sig);

}

inline void zassert(bool condition){ return ZError::zassert(condition); }
inline void zassert(bool condition, ZString message){ return ZError::zassert(condition, message); }

}

#endif // ZERROR

