/*****************************************
**               LibChaos               **
**               zmutex.h               **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZMUTEX_H
#define ZMUTEX_H

// ZMutex Class
// WARNING: Relatively untested
// Allows storing of an object in a semi-thread-safe manner.
// Thread-unique locking, so only the thread that locked the mutex is allowed to unlock it.
// While the mutex is locked, other threads may get a refrence to the contained object. THREAD RESPONSIBLY. SERIOUSLY.

#include "ztypes.h"

#if COMPILER == MSVC
    #define ZMUTEX_WINTHREADS
#endif

#ifdef ZMUTEX_WINTHREADS
    #include <windows.h>
#else
    #include <pthread.h>
#endif

#include <atomic>
#include <chrono>

namespace LibChaos {

typedef unsigned long ztid;

class ZMutex {
public:
    ZMutex() : locker_tid(0){
#ifdef ZMUTEX_WINTHREADS
        _mutex = CreateMutexA(NULL, FALSE, NULL);
#else
        pthread_mutex_init(&_mutex, NULL);
#endif
    }
    ZMutex(const ZMutex &other) : _mutex(other._mutex), locker_tid(other.locker_tid){

    }
    ~ZMutex(){
#ifdef ZMUTEX_WINTHREADS
        CloseHandle(_mutex);
#else
        pthread_mutex_destroy(&_mutex);
#endif
    }

    // If mutex is unlocked, mutex is locked by calling thread. If mutex is locked by other thread, function blocks until mutex is unlocked by other thread, then mutex is locked by calling thread.
    void lock(){
        if(!iOwn()){
#ifdef ZMUTEX_WINTHREADS
            WaitForSingleObject(_mutex, INFINITE);
            locker_tid = GetCurrentThreadId();
#else
            pthread_mutex_lock(&_mutex);
            locker_tid = pthread_self();
#endif
        }
        // We own the mutex
    }

    // Locks mutex and returns true if unlocked, else returns false.
    bool trylock(){
        if(!iOwn()){
#ifdef ZMUTEX_WINTHREADS
            if(WaitForSingleObject(_mutex, 0) == WAIT_OBJECT_0){
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

    // Tries to lock the mutex for <timeout_microsec> microseconds, then returns false.
    bool timelock(zu64 timeout_microsec){
        auto end = std::chrono::high_resolution_clock::now() + std::chrono::microseconds(timeout_microsec);
        while(!trylock()){
            // Loop until trylock() succeeds OR timeout is exceeded
            if(std::chrono::high_resolution_clock::now() > end)
                return false; // Timeout exceeded, mutex is locked by another thread
        }
        return true; // Locked, and this thread owns it
    }

    // If mutex is unlocked, returns true. If mutex is locked by calling thread, mutex is unlocked. If mutex is locked by other thread, blocks until mutex is unlocked by other thread.
    void unlock(){
        if(locked()){
            lock(); // Make sure we own the mutex first
            locker_tid = 0;
#ifdef ZMUTEX_WINTHREADS
            ReleaseMutex(_mutex);
#else
            pthread_mutex_unlock(&_mutex);
#endif
        }
        // Mutex is unlocked
    }

    // Returns true if mutex is locked, else returns false.
    inline bool locked(){
        return (bool)locker();
    }
    // Returns locking thread's id, or 0 if unlocked.
    inline ztid locker(){
        return locker_tid;
    }
    // Return true if this thread owns the mutex, else returns false
    inline bool iOwn(){
#ifdef ZMUTEX_WINTHREADS
        return (locker() == GetCurrentThreadId());
#else
        return (locker() == pthread_self());
#endif
    }

#ifdef ZMUTEX_WINTHREADS
    inline HANDLE handle(){
        return _mutex;
    }
#else
    inline pthread_mutex_t *handle(){
        return &_mutex;
    }
#endif

private:
#ifdef ZMUTEX_WINTHREADS
    HANDLE _mutex;
#else
    pthread_mutex_t _mutex;
#endif
    ztid locker_tid;
    //std::atomic<pthread_t> locker_tid;
};

// //////////////////////////////////////////////////////////////////////////////

template <class T> class ZMutexV : public ZMutex {
public:
    ZMutexV() : ZMutex(), obj(){}
    ZMutexV(const T &o) : ZMutex(), obj(o){}

    // Block until mutex owned, return refrence to obj
    T &lockdata(){
        lock();
        return obj;
    }

    // Return refrence to obj. Thread responsibly.
    inline T &data(){
        return obj;
    }

private:
    T obj;
};

} // namespace LibChaos

#endif // ZMUTEX_H
