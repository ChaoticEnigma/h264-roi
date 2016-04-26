#include "tests.h"

#include "zthread.h"
#include "zmutex.h"
#include "zlock.h"
//#include <unistd.h>

namespace LibChaosTest {

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

void thread(){
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
}

//
//
//

/**
 * @author Jeff Tanner, Seattle, jeff_tanner@earthlink.net
 *
 * Update volatile global variable until maximum value
 * is reached using critical sections.
 */

#if PLATFORM == WINDOWS

//#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
//using namespace std;

ZMutex mutex;
CRITICAL_SECTION gCS; // shared structure

const int gcMaxCount = 10;
volatile int gCount = 0;

DWORD threadLoop(void *name){
    while(true){
        TLOG((char *)name << " entering critical Section...");
//        EnterCriticalSection(&gCS);
//        mutex.lock();
        ZLock lock(mutex);
        if(gCount < gcMaxCount){
            TLOG((char *)name << " in critical Section");
            gCount++;
        } else {
//            LeaveCriticalSection(&gCS);
//            mutex.unlock();
            break;
        }
//        LeaveCriticalSection(&gCS);
//        mutex.unlock();
        TLOG((char *)name << " left critical Section");
    }
    return 0;
}

HANDLE CreateChild(const char *name){
    HANDLE hThread; DWORD dwId;
    hThread = CreateThread(NULL, 0, threadLoop, (LPVOID)const_cast<char*>(name), 0, &dwId);
    assert(hThread != NULL);
    return hThread;
}

void mutex(){
    HANDLE hT[4];
    InitializeCriticalSection(&gCS);

    TLOG("Starting...");
//    Sleep(200);

    // Create multiple child threads
    hT[0] = CreateChild("Evelyn");
    hT[1] = CreateChild("Bodie");
    hT[2] = CreateChild("Rebecca");
    hT[3] = CreateChild("Jeff");

    WaitForMultipleObjects(4, hT, TRUE, INFINITE);
    TLOG("Completed!");

    CloseHandle(hT[0]);
    CloseHandle(hT[1]);
    CloseHandle(hT[2]);
    CloseHandle(hT[3]);

    DeleteCriticalSection(&gCS);
    return 0;
}

#else

void mutex(){

}

#endif

ZArray<Test> thread_tests(){
    return {
        { "thread", thread, false, {} },
        { "mutex",  mutex,  false, {} },
    };
}

}
