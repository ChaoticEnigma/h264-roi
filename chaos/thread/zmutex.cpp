/*******************************************************************************
**                                  LibChaos                                  **
**                                  zmutex.cpp                                **
**                          (c) 2015 Zennix Studios                           **
*******************************************************************************/
#include "zmutex.h"
#include "zexception.h"

#if ZMUTEX_VERSION == 1
    #include <pthread.h>
#elif ZMUTEX_VERSION == 2 || ZMUTEX_VERSION == 4
    #include <windows.h>
#elif ZMUTEX_VERSION == 3
    #include <mutex>
#endif

#include <chrono>

namespace LibChaos {

#if ZMUTEX_VERSION == 1

ZMutex::ZMutex() : owner_tid(0){
    pthread_mutexattr_init(&_attr);
//    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&_mutex, &_attr);
}

ZMutex::~ZMutex(){
    pthread_mutex_destroy(&_mutex);
}

void ZMutex::lock(){
    if(!iOwn()){
        pthread_mutex_lock(&_mutex);
#if PLATFORM == MACOSX
        pthread_threadid_np(pthread_self(), &owner_tid);
#else
        owner_tid = pthread_self();
#endif
    }
    // We own the mutex
}

bool ZMutex::trylock(){
    if(!iOwn()){
        if(pthread_mutex_trylock(&_mutex) == 0){
#if PLATFORM == MACOSX
            pthread_threadid_np(pthread_self(), &owner_tid);
#else
            owner_tid = pthread_self();
#endif
            return true; // We now own the mutex
        }
        return false; // Another thread owns the mutex
    } else {
        return true; // We already own the mutex
    }
}

bool ZMutex::timelock(zu32 timeout_microsec){
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
#if PLATFORM == MACOSX
    ztid tid;
    pthread_threadid_np(pthread_self(), &tid);
    return (locker() == tid);
#else
    return (locker() == pthread_self());
#endif
}

#elif ZMUTEX_VERSION == 2

struct ZMutex::MutexData {
    CRITICAL_SECTION mutex;
//    CRITICAL_SECTION critical;
//    HANDLE mutex;
};

ZMutex::ZMutex() : _data(new MutexData)/*, owner_tid(0)*/{
    InitializeCriticalSection(&_data->mutex);
//    _data->mutex = CreateMutex(NULL, FALSE, NULL);
}

ZMutex::~ZMutex(){
    DeleteCriticalSection(&_data->mutex);
//    CloseHandle(_data->mutex);
}

void ZMutex::lock(){
    EnterCriticalSection(&_data->mutex);
//    while(true){
//        DWORD ret = WaitForSingleObject(_data->mutex, INFINITE);
//        if(ret == WAIT_OBJECT_0)
//            break;
//    }
    // This thread owns the mutex
}

bool ZMutex::trylock(){
    if(TryEnterCriticalSection(&_data->mutex) == TRUE){
//        owner_tid = GetCurrentThreadId();
        // This thread owns the mutex
        return true;
    } else {
        // This thread does not own the mutex
        return false;
    }
}

bool ZMutex::timelock(zu32 timeout_microsec){
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
    LeaveCriticalSection(&_data->mutex);
//    ReleaseMutex(_data->mutex);
    // Mutex is unlocked
}

//HANDLE ZMutex::handle(){
//    return (HANDLE)&_data->mutex;
////    return _data->mutex;
//}

#elif ZMUTEX_VERSION == 3

ZMutex::ZMutex() : _mutex(){

}

ZMutex::~ZMutex(){

}

void ZMutex::lock(){
    std::thread::id tid = std::this_thread::get_id();
    if(tid != _owner){
        _mutex.lock();
        _owner = tid;
    }
}

bool ZMutex::trylock(){
    std::thread::id tid = std::this_thread::get_id();
    if(tid != _owner){
        return true;
    } else {
        if(_mutex.try_lock()){
            _owner = tid;
            // This thread owns the mutex
            return true;
        } else {
            // This thread does not own the mutex
            return false;
        }
    }
}

bool ZMutex::timelock(zu32 timeout_microsec){
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
    _mutex.unlock();
    _owner = std::thread::id();
    // Mutex is unlocked
}

bool ZMutex::iOwn(){
    return (_owner == std::this_thread::get_id());
}

bool ZMutex::locked(){
    return (_owner != std::thread::id());
}

ztid ZMutex::locker(){
    return _owner.hash();
}

#elif ZMUTEX_VERSION == 4

struct ZMutex::MutexData {
    HANDLE mutex;
    CRITICAL_SECTION critical;
    DWORD owner;
    zu32 depth;
};

ZMutex::ZMutex() : _data(new MutexData){
    _data->owner = 0;
    _data->depth = 0;
    _data->mutex = CreateMutex(NULL, FALSE, NULL);
    InitializeCriticalSection(&_data->critical);
}

ZMutex::~ZMutex(){
    DeleteCriticalSection(&_data->critical);
    CloseHandle(_data->mutex);
}

void ZMutex::lock(){
    EnterCriticalSection(&_data->critical);
    DWORD tid = GetCurrentThreadId();
    if(tid != _data->owner){
        // This thread does not own the mutex
        LeaveCriticalSection(&_data->critical);
        while(true){
            DWORD ret = WaitForSingleObject(_data->mutex, INFINITE);
            if(ret == WAIT_OBJECT_0){
                // This thread owns the mutex
                EnterCriticalSection(&_data->critical);
                _data->owner = tid;
                break;
            }
        }
    }
    // This thread owns the mutex
    _data->depth++;
    LeaveCriticalSection(&_data->critical);
}

bool ZMutex::trylock(){
    EnterCriticalSection(&_data->critical);
    DWORD tid = GetCurrentThreadId();
    if(tid != _data->owner){
        // This thread does not own the mutex
        LeaveCriticalSection(&_data->critical);
        while(true){
            DWORD ret = WaitForSingleObject(_data->mutex, 0);
            if(ret == WAIT_OBJECT_0){
                // This thread owns the mutex
                EnterCriticalSection(&_data->critical);
                _data->owner = tid;
                break; // Mutex was locked
            } else if(ret == WAIT_TIMEOUT){
                return false; // Mutex was not locked
            }
        }
    }
    // This thread owns the mutex
    _data->depth++;
    LeaveCriticalSection(&_data->critical);
    return true;
}

bool ZMutex::timelock(zu32 milliseconds){
    EnterCriticalSection(&_data->critical);
    DWORD tid = GetCurrentThreadId();
    if(tid != _data->owner){
        LeaveCriticalSection(&_data->critical);
        while(true){
            DWORD ret = WaitForSingleObject(_data->mutex, milliseconds);
            if(ret == WAIT_OBJECT_0){
                EnterCriticalSection(&_data->critical);
                _data->owner = tid;
                break; // Mutex was locked
            } else if(ret == WAIT_TIMEOUT){
                return false; // Mutex was not locked
            }
        }
    }
    _data->depth++;
    LeaveCriticalSection(&_data->critical);
    return true;
}

void ZMutex::unlock(){
    EnterCriticalSection(&_data->critical);
    DWORD tid = GetCurrentThreadId();
    if(tid != _data->owner){
        LeaveCriticalSection(&_data->critical);
        lock(); // Make sure we own the mutex first
        EnterCriticalSection(&_data->critical);
    }
    _data->depth--;
    if(_data->depth == 0){
        ReleaseMutex(_data->mutex);
        // Mutex is unlocked
        _data->owner = 0;
    }
    LeaveCriticalSection(&_data->critical);
}

#endif // ZMUTEX_VERSION

}
