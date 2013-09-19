#ifndef ZMUTEX_H
#define ZMUTEX_H

// ZMutex Class
// WARNING: Relatively untested
// Allows storing of an object in a thread-safe manner.
// Thread-specifc locking, so only the thread that locked the mutex is allowed to unlock it or get a refrence to the contained object.
// While the mutex is locked, other threads may retrieve a copy of the contained object AT THEIR OWN RISK.
//      If the object is copied while being written to by another thread, corruption of the copied object may occur.

// read() Returns a read-only COPY of the object.
// data() If calling thread is locking thread, refrence to contained object is returned, else function blocks until mutex is unlocked.
// lock() If mutex is unlocked, mutex is locked by calling thread. If mutex is locked by other thread, function blocks until mutex is unlocked by other thread, then mutex is locked by calling thread.
// unlock() If mutex is unlocked, returns true. If mutex is locked by calling thread, mutex is unlocked. If mutex is locked by other thread, blocks until mutex is unlocked by other thread.
// locked() Returns true if mutex is locked. Else returns false;
// locker() Returns copy of locking thread's id, or empty boost::thread::id object if unlocked.

//#include "boost/thread.hpp"
#include "zthread.h"
#include <atomic>

namespace LibChaos {

template <class T> class ZMutex {
public:
    ZMutex() : lock_(false), obj(){}
    T read(){
        return obj;
    }
    T &data(){
        //if(tid != boost::this_thread::get_id()){
        if(tid != ZThread::thisTid()){
            while(lock_){
                // Loop until lock_ = false
            }
        }
        return obj;
    }
    bool lock(){
        //if(tid == boost::this_thread::get_id()){
        if(tid == ZThread::thisTid()){
            return false;
        } else {
            while(lock_){
                // Loop until lock_ = false
            }
            //tid = boost::this_thread::get_id();
            tid = ZThread::thisTid();
            lock_ = true;
        }
        return true;
    }
    bool unlock(){
        //if(tid == boost::this_thread::get_id()){
        if(tid == ZThread::thisTid()){
            //tid = boost::thread::id();
            tid = 0;
            lock_ = false;
        } else {
            while(lock_){
                //sleep(1);
            }
        }
        return true;
    }
    bool locked(){
        return lock_;
    }
    //boost::thread::id locker(){
    ztid locker(){
        return tid;
    }

private:
    std::atomic_bool lock_;
    ztid tid;
    T obj;
};

} // namespace LibChaos

#endif // ZMUTEX_H
