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

bool ZThread::ZThreadContainer::stop() const {
    return _stop;
}

void *ZThread::ZThreadCallback::run(void *arg){
    // Default run() executes callback
    if(arg){
        ZThreadArg zarg;
        zarg.arg = arg;
        zarg.tc = this;
        return func(zarg);
    }
    return nullptr;
}

ZThread::ZThread(ZThreadContainer *threadcont) : _thread(0), _container(threadcont), _alive(false){

}

ZThread::ZThread(funcType func) : ZThread(new ZThreadCallback(func)){

}

ZThread::~ZThread(){
    // Detach thread handle
    detach();
}

bool ZThread::exec(void *user){
    zthreadparam *param = new zthreadparam;
    param->container = _container;
    param->arg = user;

#ifdef ZTHREAD_WINTHREADS
    _thread = CreateThread(NULL, 0, _entry_win, param, 0, NULL);
    if(_thread == NULL)
        return false;
#else
    // Start thread
    int ret = ::pthread_create(&_thread, NULL, _entry_posix, param);
    if(ret != 0){
        _thread = 0;
        return false;
    }
#endif
    return true;
}

void *ZThread::join(){
#ifdef ZTHREAD_WINTHREADS
    if(_thread){
        WaitForSingleObject(_thread, INFINITE);
        _thread = NULL;
    }
    return nullptr;
#else
    void *retval = nullptr;
    if(_thread){
        // Wait for thread to finish, and get return value
        ::pthread_join(_thread, &retval);
        _thread = 0;
    }
    return retval;
#endif
}

void ZThread::kill(){
#ifdef ZTHREAD_WINTHREADS
    if(_thread){
        TerminateThread(_thread, 0);
        _thread = NULL;
    }
#else
    if(_thread){
        // Mark thread for cancellation
        ::pthread_cancel(_thread);
        _thread = 0;
    }
#endif
}

void ZThread::stop(){
    if(_container){
        _container->_stop = true;
    }
}

void ZThread::detach(){
#ifdef ZTHREAD_WINTHREADS
    if(_thread){
        CloseHandle(_thread);
        _thread = NULL;
    }
#else
    if(_thread){
        // Mark thread detached
        ::pthread_detach(_thread);
        _thread = 0;
    }
#endif
}

bool ZThread::alive(){
    return _alive;
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

void ZThread::yield(){
#ifdef ZTHREAD_WINTHREADS
    SwitchToThread();
#elif PLATFORM == WINDOWS
    std::this_thread::yield();
#elif PLATFORM == MACOSX
    sched_yield();
#else
    ::pthread_yield();
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
    ft.QuadPart = ((LONGLONG)microseconds * 10); // Convert to 100 nanosecond interval, negative value indicates relative time
    HANDLE timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
#else
    ::usleep(microseconds);
#endif
}

void *ZThread::_entry_common(zthreadparam *param){
    void *ret = nullptr;
    if(param){
        param->container->_stop = false;
        param->container->_alive = true;
        ret = param->container->run(param->arg);
        param->container->_alive = false;
    }
    param->container->_alive = false;

    return ret;
}

#ifdef ZTHREAD_WINTHREADS

#if COMPILER == MSVC
DWORD _stdcall ZThread::_entry_win(LPVOID ptr){
#else
DWORD __attribute__((__stdcall__)) ZThread::_entry_win(LPVOID ptr){
#endif
    // Now running on executing thread
    zthreadparam *param = (zthreadparam *)ptr;
    if(param){
        void *ret = _entry_common(param);
        delete param;
    }
    return 0;
}

#else

void *ZThread::_entry_posix(void *ptr){
    // Now running on executing thread
    zthreadparam *param = (zthreadparam *)ptr;
    if(param){
        void *ret = _entry_common(param);
        delete param;
        return ret;
    } else {
        return nullptr;
    }
}

#endif

} // namespace LibChaos
