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

namespace LibChaos {

bool ZThreadArg::stop(){
    return (bool)*_stop;
}

ZThread::ZThread() :  _thread(0), _stop(false), _alive(false), _copyable(false){

}
ZThread::ZThread(funcType func) : ZThread(){
    run(func);
}
ZThread::ZThread(funcType func, void *argptr) : ZThread(){
    run(func, argptr);
}
ZThread::ZThread(const ZThread &other) : _thread(other._thread), _param(other._param), _stop((bool)other._stop),
                                         _return(other._return), _alive(other._alive), _copyable(other._copyable){

}

ZThread::~ZThread(){
    if (!_copyable){
        detach();
    }
}

#ifdef ZTHREAD_WINTHREADS

#if COMPILER == MSVC
DWORD _stdcall ZThread::entry_win(LPVOID ptr){
#else
DWORD ZThread::entry_win(LPVOID ptr){
#endif
    ZThread *thr = (ZThread *)ptr;
    thr->_param.funcptr(&thr->_param.zarg); // Run function
    thr->_alive = false;
    return 0;
}

#else

void *ZThread::entry_posix(void *ptr){
    ZThread *thr = (ZThread*)ptr;
    void *ret = thr->_param.funcptr(&thr->_param.zarg);
    //pthread_exit(ret);
    thr->_alive = false;
    return ret;
}

#endif

bool ZThread::run(funcType func){
    return run(func, NULL);
}
bool ZThread::run(funcType func, void *argptr){
    _param.zarg._stop = &_stop;
    _param.funcptr = func;
    _param.zarg.arg = argptr;

#ifdef ZTHREAD_WINTHREADS
    _thread = CreateThread(NULL, 0, entry_win, this, 0, NULL);
    if(_thread == NULL)
        return false;
#else
    _return = pthread_create(&_thread, NULL, entry_posix, this);
    //std::cout << "create " << ret << std::endl;
    if(_return != 0)
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
    void *retval = NULL;
    if(_alive)
        _return = pthread_join(_thread, &retval);
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
    if(_alive){
        _return = pthread_cancel(_thread);
        _alive = false;
    }
#endif
}

void ZThread::stop(){
    _stop = true;
}

void ZThread::detach(){
#ifdef ZTHREAD_WINTHREADS
    if(_alive)
        CloseHandle(_thread);
#else
    if(_alive)
        _return = pthread_detach(_thread);
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

void ZThread::sleep(zu64 seconds){
#ifdef ZTHREAD_WINTHREADS
    Sleep((DWORD)(seconds * 1000));
#else
    ::sleep(seconds);
#endif
}

void ZThread::msleep(zu64 milliseconds){
#ifdef ZTHREAD_WINTHREADS
    Sleep((DWORD)milliseconds);
#else
    ::usleep(milliseconds * 1000);
#endif
}

void ZThread::usleep(zu64 microseconds){
#ifdef ZTHREAD_WINTHREADS
    LARGE_INTEGER ft;
    ft.QuadPart = (10 * microseconds); // Convert to 100 nanosecond interval, negative value indicates relative time
    HANDLE timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
#else
    ::usleep(microseconds);
#endif
}

void ZThread::setCopyable(){
    _copyable = true;
}

ztid ZThread::tid(){
    return (ztid)_thread;
}
bool ZThread::alive(){
    return _alive;
}

ztid ZThread::thisTid(){
#ifdef ZTHREAD_WINTHREADS
    return (ztid)GetCurrentThreadId();
#else
    return (ztid)pthread_self();
#endif
}

}
