/*****************************************
**               LibChaos               **
**               zthread.h              **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZTHREAD_H
#define ZTHREAD_H

#include "ztypes.h"

#if COMPILER == MSVC
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
    static void sleep(zu64 seconds);
    static void msleep(zu64 milliseconds);
    static void usleep(zu64 microseconds);

    void setCopyable();

    ztid tid();
    bool alive();
    static ztid thisTid();

private:
#ifdef ZTHREAD_WINTHREADS
    static DWORD entry_win(LPVOID ptr);
#else
    static void *entry(void *ptr);
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
    int ret;
    bool _alive;
    bool copyable;
};

typedef ZThread ZThreadA;

} // namespace LibChaos

#endif // ZTHREAD_H
