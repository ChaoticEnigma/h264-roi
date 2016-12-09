/*******************************************************************************
**                                  LibChaos                                  **
**                                 zthread.cpp                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zthread.h"

#ifdef ZTHREAD_WINTHREADS
    #include <windows.h>
#else
    #if PLATFORM == MACOSX
        #include <sched.h>
    #endif
    #include <signal.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <pthread.h>

    #include <thread>
#endif

#ifdef ZTHREAD_WINTHREADS

#if COMPILER == MSVC
DWORD _stdcall entry_win(LPVOID ptr){
#else
DWORD entry_win(LPVOID ptr){
#endif

#else
void *entry_posix(void *ptr){
#endif

    using namespace LibChaos;
    ZThread::zthreadparam *param = (ZThread::zthreadparam *)ptr;

    param->mutex.lock();
    if(param->handle)
        param->handle->_setThreadAlive(true);
    param->mutex.unlock();

    // Run thread function
    void *ret = param->func(&param->zarg);

    param->mutex.lock();
    if(param->handle)
        param->handle->_setThreadAlive(false);
    param->mutex.unlock();

#ifdef ZTHREAD_WINTHREADS
    return 0;
#else
    return ret;
#endif
}

namespace LibChaos {

bool ZThreadArg::stop(){
    return _stop;
}

ZThread::ZThread() : _thread(0), _param(nullptr), _alive(false), _copyable(false){

}

ZThread::ZThread(funcType func) : ZThread(){
    run(func);
}

ZThread::ZThread(funcType func, void *user) : ZThread(){
    run(func, user);
}

ZThread::~ZThread(){
    detach();
}

bool ZThread::run(funcType func){
    return run(func, nullptr);
}

bool ZThread::run(funcType func, void *user){
    _param = new zthreadparam;
    _param->zarg._stop = false;
    _param->func = func;
    _param->zarg.arg = user;

#ifdef ZTHREAD_WINTHREADS
    _thread = CreateThread(NULL, 0, entry_win, _param, 0, NULL);
    if(_thread == NULL)
        return false;
#else
    int ret = pthread_create(&_thread, NULL, entry_posix, _param);
    if(ret != 0)
        return false;
#endif
    return true;
}

void *ZThread::join(){
#ifdef ZTHREAD_WINTHREADS
    if(_alive)
        WaitForSingleObject(_thread, INFINITE);
    return NULL;
#else
    void *retval = nullptr;
    if(_thread && _alive){
        pthread_join(_thread, &retval);
    }
    return retval;
#endif
}

void ZThread::kill(){
#ifdef ZTHREAD_WINTHREADS
    if(_alive){
        TerminateThread(_thread, 0);
        _alive = false;
    }
#else
    if(_thread && _alive){
        pthread_cancel(_thread);
        _param->mutex.lock();
        _param->handle = nullptr;
        _param->mutex.unlock();
        _alive = false;
        _thread = 0;
    }
#endif
}

void ZThread::stop(){
    if(_param){
        _param->zarg._stop = true;
    }
}

void ZThread::detach(){
#ifdef ZTHREAD_WINTHREADS
    if(_alive)
        CloseHandle(_thread);
#else
    if(_thread && _alive){
        pthread_detach(_thread);
        _param->mutex.lock();
        _param->handle = nullptr;
        _param->mutex.unlock();
        _alive = false;
        _thread = 0;
    }
#endif
}

void ZThread::yield(){
#ifdef ZTHREAD_WINTHREADS
    SwitchToThread();
#else
    #if PLATFORM == WINDOWS
        std::this_thread::yield();
    #elif PLATFORM == MACOSX
        sched_yield();
    #else
        pthread_yield();
    #endif
#endif
}

void ZThread::sleep(zu32 seconds){
#ifdef ZTHREAD_WINTHREADS
    Sleep((DWORD)(seconds * 1000));
#else
    ::sleep(seconds);
#endif
}

void ZThread::msleep(zu32 milliseconds){
#ifdef ZTHREAD_WINTHREADS
    Sleep((DWORD)milliseconds);
#else
    ::usleep(milliseconds * 1000);
#endif
}

void ZThread::usleep(zu32 microseconds){
#ifdef ZTHREAD_WINTHREADS
    LARGE_INTEGER ft;
    ft.QuadPart = (10 * (LONGLONG)microseconds); // Convert to 100 nanosecond interval, negative value indicates relative time
    HANDLE timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
#else
    ::usleep(microseconds);
#endif
}

ztid ZThread::tid(){
    return (ztid)_thread;
}

ztid ZThread::thisTid(){
#ifdef ZTHREAD_WINTHREADS
    return (ztid)GetCurrentThreadId();
#else
    return (ztid)pthread_self();
#endif
}

bool ZThread::alive(){
    return _alive;
}

void ZThread::_setThreadAlive(bool alive){
    _alive = alive;
}

}
