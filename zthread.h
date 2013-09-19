#ifndef ZTHREAD_H
#define ZTHREAD_H

#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
//#include <atomic>

namespace LibChaos {

typedef unsigned long ztid;

struct ZThreadArg {
    bool stop();
    void *arg;
    bool *_stop;
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

    static void *entry(void *ptr);

    ztid tid();
    static ztid thisTid();
private:
    struct zthreadparam {
        funcType funcptr;
        ZThreadArg *zarg;
    };

    bool _run;
    bool _stop;
    int ret;
    pthread_t thread;
};

typedef ZThread ZThreadA;

} // namespace LibChaos

#endif // ZTHREAD_H
