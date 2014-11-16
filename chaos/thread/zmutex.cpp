#include "zmutex.h"
#include "zerror.h"

#ifdef ZMUTEX_WINTHREADS
    #include <windows.h>
#else
    #include <pthread.h>
#endif

#include <chrono>

namespace LibChaos {

#ifdef ZMUTEX_WINTHREADS

struct ZMutex::MutexData {
    CRITICAL_SECTION crit;
};

ZMutex::ZMutex() : _mutex(new MutexData)/*, owner_tid(0)*/{
    InitializeCriticalSection(&_mutex->crit);
}

ZMutex::~ZMutex(){
    DeleteCriticalSection(&_mutex->crit);
}

void ZMutex::lock(){
    EnterCriticalSection(&_mutex->crit);
    // This thread owns the mutex
}

bool ZMutex::trylock(){
    if(TryEnterCriticalSection(&_mutex->crit) == TRUE){
//        owner_tid = GetCurrentThreadId();
        // This thread owns the mutex
        return true;
    } else {
        // This thread does not own the mutex
        return false;
    }
}

bool ZMutex::timelock(zu64 timeout_microsec){
    auto end = std::chrono::high_resolution_clock::now() + std::chrono::microseconds(timeout_microsec);
    while(!trylock()){
        // Loop until trylock() succeeds OR timeout is exceeded
        if(std::chrono::high_resolution_clock::now() > end)
            return false; // Timeout exceeded, mutex is locked by another thread
    }
    // This thread owns the mutex
    return true;
}

void ZMutex::unlock(){
    lock(); // Make sure we own the mutex first
    // This thread owns the mutex
//    owner_tid = 0;
    LeaveCriticalSection(&_mutex->crit);
    // Mutex is unlocked
}

CRITICAL_SECTION *ZMutex::handle(){
    return &_mutex->crit;
}

#else // ZMUTEX_WINTHREADS

ZMutex::ZMutex() : owner_tid(0){
    pthread_mutex_init(&_mutex, NULL);
}

ZMutex::~ZMutex(){
    pthread_mutex_destroy(&_mutex);
}

void ZMutex::lock(){
    if(!iOwn()){
        pthread_mutex_lock(&_mutex);
        owner_tid = pthread_self();
    }
    // We own the mutex
}

bool ZMutex::trylock(){
    if(!iOwn()){
        if(pthread_mutex_trylock(&_mutex) == 0){
            owner_tid = pthread_self();
            return true; // We now own the mutex
        }
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
        owner_tid = 0;
        pthread_mutex_unlock(&_mutex);
    }
    // Mutex is unlocked
}

bool ZMutex::iOwn(){
    return (locker() == pthread_self());
}

#endif // ZMUTEX_WINTHREADS

}
