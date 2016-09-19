/*******************************************************************************
**                                  LibChaos                                  **
**                                  zthread.h                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZTHREAD_H
#define ZTHREAD_H

#include "ztypes.h"

#if PLATFORM == WINDOWS || PLATFORM == CYGWIN
    #define ZTHREAD_WINTHREADS
#endif

#ifndef ZTHREAD_WINTHREADS
    #include <pthread.h>
#endif

#include <atomic>

#undef bool // WTF? This is sometimes necessary?

namespace LibChaos {

typedef zu64 ztid;

struct ZThreadArg {
    bool stop();
    void *arg;
    std::atomic<bool> *_stop;
};

//! Cross-platform/API thread handle abstraction.
class ZThread {
public:
    typedef void *(*funcType)(void *);

#ifdef ZTHREAD_WINTHREADS
private:
    typedef unsigned long DWORD;
    typedef void *LPVOID;
#endif

public:
    ZThread();
    ZThread(funcType);
    ZThread(funcType, void *);
    ZThread(const ZThread &other);

    ~ZThread();

    bool run(funcType fnc);
    bool run(funcType fnc, void *arg);
    void *join();
    void kill();
    void stop();
    void detach();

    static void yield();
    static void sleep(zu32 seconds);
    static void msleep(zu32 milliseconds);
    static void usleep(zu32 microseconds);

    void setCopyable();

    ztid tid();
    bool alive();
    static ztid thisTid();

private:
#ifdef ZTHREAD_WINTHREADS
#if COMPILER == MSVC
    static DWORD _stdcall entry_win(LPVOID ptr);
#else
    static DWORD __attribute__((__stdcall__)) entry_win(LPVOID ptr);
#endif
#else
    static void *entry_posix(void *ptr);
#endif

private:
    struct zthreadparam {
        funcType funcptr;
        ZThreadArg zarg;
    };

    typedef void * HANDLE;

private:
#ifdef ZTHREAD_WINTHREADS
    HANDLE _thread;
#else
    pthread_t _thread;
#endif
    zthreadparam _param;
    std::atomic<bool> _stop;
    int _return;
    bool _alive;
    bool _copyable;
};

typedef ZThread ZThreadA;

} // namespace LibChaos

#endif // ZTHREAD_H
