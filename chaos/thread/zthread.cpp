#include "zthread.h"

#if COMPILER == MSVC
    #include <windows.h>
#else
    #include <signal.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <thread>
#endif

namespace LibChaos {

bool ZThreadArg::stop(){
    return (bool)*_stop;
}

ZThread::ZThread() : _stop(false), _thread(0), _alive(false), copyable(false){

}
ZThread::ZThread(funcType func) : ZThread(){
    run(func);
}
ZThread::ZThread(funcType func, void *argptr) : ZThread(){
    run(func, argptr);
}
ZThread::ZThread(const ZThread &other) :_param(other._param), _stop((bool)other._stop), ret(other.ret), _thread(other._thread), _alive(other._alive), copyable(other.copyable){

}

ZThread::~ZThread(){
    if(!copyable)
        detach();
}

void *ZThread::entry(void *ptr){
    ZThread *thr = (ZThread*)ptr;
    void *ret = thr->_param.funcptr(&thr->_param.zarg);
    //pthread_exit(ret);
    thr->_alive = false;
    return ret;
}

bool ZThread::run(funcType func){
    return run(func, NULL);
}
bool ZThread::run(funcType func, void *argptr){
    _param.zarg._stop = &_stop;
    _param.funcptr = func;
    _param.zarg.arg = argptr;

    ret = pthread_create(&_thread, NULL, entry, this);
    //std::cout << "create " << ret << std::endl;
    if(ret == 0){
        _alive = true;
    } else {
        return false;
    }
    return true;
}

void *ZThread::join(){
    void *retval = NULL;
    if(_alive)
        ret = pthread_join(_thread, &retval);
    return retval;
}

void ZThread::kill(){
    if(_alive){
        ret = pthread_cancel(_thread);
        _alive = false;
    }
}

void ZThread::stop(){
    _stop = true;
}

void ZThread::detach(){
    if(_alive)
        ret = pthread_detach(_thread);
}

void ZThread::setCopyable(){
    copyable = true;
}

ztid ZThread::tid(){
    return (ztid)_thread;
}
bool ZThread::alive(){
    return _alive;
}

ztid ZThread::thisTid(){
    return (ztid)pthread_self();
}

void ZThread::yield(){
#if PLATFORM == WINDOWS
    std::this_thread::yield();
#else
    pthread_yield();
#endif
}

}
