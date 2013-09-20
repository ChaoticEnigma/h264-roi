#include "zthread.h"
#include <signal.h>
#include <iostream>

namespace LibChaos {

bool ZThreadArg::stop(){
    return (bool)*_stop;
}

ZThread::ZThread() : _run(false), _stop(false){}
ZThread::ZThread(funcType func) : _run(false), _stop(false){
    run(func);
}
ZThread::ZThread(funcType func, void *argptr) : _run(false), _stop(false){
    run(func, argptr);
}

ZThread::~ZThread(){
    //kill();
    //stop();
    detach();
}

void *ZThread::entry(void *ptr){
    zthreadparam *param = (zthreadparam*) ptr;
    param->funcptr(param->zarg);
    return NULL;
}

bool ZThread::run(funcType func){
    return run(func, NULL);
}
bool ZThread::run(funcType func, void *argptr){
    zthreadparam *param  = new zthreadparam;
    param->funcptr = func;
    param->zarg = new ZThreadArg;
    param->zarg->arg = argptr;
    bool *sptr = &_stop;
    param->zarg->_stop = sptr;

    ret = pthread_create(&thread, NULL, entry, param);
    //std::cout << "create " << ret << std::endl;
    if(ret != 0)
        return false;
    _run = true;
    return true;
}

void *ZThread::join(){
    void *retval = NULL;
    ret = pthread_join(thread, &retval);
    //std::cout << "join " << ret << std::endl;
    _run = false;
    return retval;
}

void ZThread::kill(){
    //if(_run){
        ret = pthread_cancel(thread);
        //std::cout << "cancel " << ret << std::endl;
    //}
    _run = false;
    //pthread_exit(thread);
}

void ZThread::stop(){
    //ret = pthread_kill(thread, SIGINT);
    _stop = true;
    _run = false;
    //std::cout << "stop" << std::endl;
}

void ZThread::detach(){
    ret = pthread_detach(thread);
    //std::cout << "detach " << ret << std::endl;
}

ztid ZThread::tid(){
    return (ztid) thread;
}
ztid ZThread::thisTid(){
    return (ztid) pthread_self();
}


}
