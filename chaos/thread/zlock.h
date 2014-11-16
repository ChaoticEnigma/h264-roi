#ifndef ZLOCK_H
#define ZLOCK_H

#include "zmutex.h"

namespace LibChaos {

class ZLock {
public:
    ZLock(ZMutex &mutex) : _mutex(&mutex){
        _mutex->lock();
    }
    ~ZLock(){
        _mutex->unlock();
    }

    void lock(){
        _mutex->lock();
    }
    void unlock(){
        _mutex->unlock();
    }

private:
    ZMutex *_mutex;
};

}

#endif // ZLOCK_H
