/*****************************************
**               LibChaos               **
**               zthread.h              **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZTHREAD_H
#define ZTHREAD_H

#include <pthread.h>

#include <atomic>
// WTF ?
#undef bool

#include "ztypes.h"

namespace LibChaos {

typedef unsigned long ztid;

struct ZThreadArg {
    bool stop();
    void *arg;
    std::atomic<bool> *_stop;
};

class ZThread {
public:
    typedef void *(*funcType)(void *);

    ZThread();
    ZThread(funcType);
    ZThread(funcType, void *);
    ~ZThread();

    bool run(funcType fnc);
    bool run(funcType fnc, void *arg);
    void *join();
    void kill();
    void stop();
    void detach();

    static void yield();

    static void *entry(void *ptr);

    ztid tid();
    bool alive();
    static ztid thisTid();

    struct zthreadparam {
        funcType funcptr;
        ZThreadArg zarg;
    } _param;
private:
    std::atomic<bool> _stop;
    int ret;
    pthread_t thread;
    bool _alive;
};

typedef ZThread ZThreadA;

} // namespace LibChaos

#endif // ZTHREAD_H
