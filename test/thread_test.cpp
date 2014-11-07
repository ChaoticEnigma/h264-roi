#include "test.h"

#include "zthread.h"
//#include <unistd.h>

void *thread_func(void * /*zarg*/){
    //ZThreadArg *arg = (ZThreadArg*)zarg;
    LOG("running " << ZThread::thisTid());
    ZThread::sleep(2);
    LOG("waited 2 " << ZThread::thisTid());
    return NULL;
}
void *thread_func2(void *zarg){
    ZThreadArg *arg = (ZThreadArg*)zarg;
    LOG("running " << ZThread::thisTid());
    LOG((const char *)arg->arg << ", " << arg->stop());
    int i = 0;
    while(!arg->stop()){
        LOG("loop" << ++i << " in " << ZThread::thisTid());
        ZThread::usleep(1000000);
    }
    LOG("broke loop " << ZThread::thisTid());
    return NULL;
}

int thread_block(){
    LOG("=== Thread Test...");
    /*
    LOG("this text " << ZThread::thisTid());
    ZThread thr(thread_func);
    LOG("thread " << thr.tid() << " created");
    sleep(1);
    LOG("waited 1 " << ZThread::thisTid());
    thr.kill();
    LOG("killed " << thr.tid());
    */

    ZString txt = "hello there from here";
    ZThread thr2(thread_func2, txt.c());
    LOG("thread " << thr2.tid() << " created");
    ZThread::sleep(5);
    LOG("waited 5 " << ZThread::thisTid());
    thr2.stop();
    LOG("stopped " << thr2.tid());
    thr2.join();
    LOG("joined " << thr2.tid());
    return 0;
}
