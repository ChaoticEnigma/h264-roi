/*******************************************************************************
**                                  LibChaos                                  **
**                                  zclock.h                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZLOCK_H
#define ZLOCK_H

#include "zmutex.h"

namespace LibChaos {

/*! ZLock locks a mutex when constructed, and unlocks it when destroyed.
 *  \ingroup Thread
 */
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

} // namespace LibChaos

#endif // ZLOCK_H
