#include "zthread.h"

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <thread>
#include <iostream>

namespace LibChaos {

bool ZThreadArg::stop(){
    return (bool)*_stop;
}

ZThread::ZThread() : _stop(false), thread(0), _alive(false){
    _param.zarg._stop = &_stop;
}
ZThread::ZThread(funcType func) : _stop(false), thread(0){
    _param.zarg._stop = &_stop;
    run(func);
}
ZThread::ZThread(funcType func, void *argptr) : _stop(false), thread(0){
    _param.zarg._stop = &_stop;
    run(func, argptr);
}

ZThread::~ZThread(){
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
    _param.funcptr = func;
    _param.zarg.arg = argptr;

    ret = pthread_create(&thread, NULL, entry, this);
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
        ret = pthread_join(thread, &retval);
    return retval;
}

void ZThread::kill(){
    if(_alive){
        ret = pthread_cancel(thread);
        _alive = false;
    }
}

void ZThread::stop(){
    _stop = true;
}

void ZThread::detach(){
    if(_alive)
        ret = pthread_detach(thread);
}

ztid ZThread::tid(){
    return (ztid)thread;
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
