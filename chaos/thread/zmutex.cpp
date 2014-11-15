#include "zmutex.h"
#include "zerror.h"

#ifdef ZMUTEX_WINTHREADS
    #include <windows.h>
#else
    #include <pthread.h>
#endif

#include <chrono>

namespace LibChaos {

struct ZMutex::MutexData {
    CRITICAL_SECTION crit;
};

#ifdef ZMUTEX_WINTHREADS
ZMutex::ZMutex() : _mutex(new MutexData)/*, owner_tid(0)*/{
    InitializeCriticalSection(&_mutex->crit);
}
#else
ZMutex::ZMutex() : locker_tid(0){
    pthread_mutex_init(&_mutex, NULL);
}
#endif

ZMutex::ZMutex(const ZMutex &){
    throw ZError("Someone tried to copy a ZMutex");
}

ZMutex::~ZMutex(){
#ifdef ZMUTEX_WINTHREADS
    DeleteCriticalSection(&_mutex->crit);
#else
    pthread_mutex_destroy(&_mutex);
#endif
}

void ZMutex::lock(){
#ifdef ZMUTEX_WINTHREADS
    EnterCriticalSection(&_mutex->crit);
//    owner_tid = GetCurrentThreadId();
#else
    if(!iOwn()){
        pthread_mutex_lock(&_mutex);
        locker_tid = pthread_self();
    }
#endif
    // We own the mutex
}

bool ZMutex::trylock(){
#ifdef ZMUTEX_WINTHREADS
    if(TryEnterCriticalSection(&_mutex->crit) == TRUE){
//        owner_tid = GetCurrentThreadId();
        return true;
    } else {
        return false;
    }
#else
    if(!iOwn()){
        if(pthread_mutex_trylock(&_mutex) == 0){
            locker_tid = pthread_self();
            return true; // We now own the mutex
        }
        return false; // Another thread owns the mutex
    } else {
        return true; // We already own the mutex
    }
#endif
}

bool ZMutex::timelock(zu64 timeout_microsec){
    auto end = std::chrono::high_resolution_clock::now() + std::chrono::microseconds(timeout_microsec);
    while(!trylock()){
        // Loop until trylock() succeeds OR timeout is exceeded
        if(std::chrono::high_resolution_clock::now() > end)
            return false; // Timeout exceeded, mutex is locked by another thread
    }
    return true; // Locked, and this thread owns it
}

void ZMutex::unlock(){
#ifdef ZMUTEX_WINTHREADS
    lock(); // Make sure we own the mutex first
//    owner_tid = 0;
    LeaveCriticalSection(&_mutex->crit);
#else
    if(locked()){
        lock(); // Make sure we own the mutex first
        locker_tid = 0;
        pthread_mutex_unlock(&_mutex);
    }
#endif
    // Mutex is unlocked
}

CRITICAL_SECTION *ZMutex::handle(){
    return &_mutex->crit;
}

#ifndef ZMUTEX_WINTHREADS
bool ZMutex::iOwn(){
    return (locker() == pthread_self());
}
#endif

}
