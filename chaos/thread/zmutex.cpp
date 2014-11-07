#include "zmutex.h"
#include "zerror.h"

#ifdef ZMUTEX_WINTHREADS
    #include <windows.h>
#else
    #include <pthread.h>
#endif

namespace LibChaos {

#ifdef ZMUTEX_WINTHREADS
ZMutex::ZMutex() : _mutex(new CRITICAL_SECTION), locker_tid(0){
    InitializeCriticalSection(_mutex);
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
    DeleteCriticalSection(_mutex);
#else
    pthread_mutex_destroy(&_mutex);
#endif
}

void ZMutex::lock(){
    if(!iOwn()){
#ifdef ZMUTEX_WINTHREADS
        EnterCriticalSection(_mutex);
        locker_tid = GetCurrentThreadId();
#else
        pthread_mutex_lock(&_mutex);
        locker_tid = pthread_self();
#endif
    }
    // We own the mutex
}

bool ZMutex::trylock(){
    if(!iOwn()){
#ifdef ZMUTEX_WINTHREADS
        if(TryEnterCriticalSection(_mutex)){
            locker_tid = GetCurrentThreadId();
            return true; // We now own the mutex
        }
#else
        if(pthread_mutex_trylock(&_mutex) == 0){
            locker_tid = pthread_self();
            return true; // We now own the mutex
        }
#endif
        return false; // Another thread owns the mutex
    } else {
        return true; // We already own the mutex
    }
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
    if(locked()){
        lock(); // Make sure we own the mutex first
        locker_tid = 0;
#ifdef ZMUTEX_WINTHREADS
        LeaveCriticalSection(_mutex);
#else
        pthread_mutex_unlock(&_mutex);
#endif
    }
    // Mutex is unlocked
}

bool ZMutex::iOwn(){
#ifdef ZMUTEX_WINTHREADS
    return (locker() == GetCurrentThreadId());
#else
    return (locker() == pthread_self());
#endif
}

}
