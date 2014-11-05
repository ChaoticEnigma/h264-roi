#ifndef ZCONDITION_H
#define ZCONDITION_H

#include "ztypes.h"
#include "zmutex.h"

namespace LibChaos {

class ZCondition {
public:
    ZCondition() : mutex(){
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cond, NULL);
    }
    ~ZCondition(){
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }

    void waitOnce(){
        pthread_mutex_lock(&mutex);
        //mutex.lock();
        //pthread_cond_wait(&cond, mutex.handle());
        pthread_cond_wait(&cond, &mutex);
        //mutex.unlock();
        pthread_mutex_unlock(&mutex);
    }

    void waitLock(){
        //mutex.lock();
        pthread_mutex_lock(&mutex);
    }
    void wait(){
        //pthread_cond_wait(&cond, mutex.handle());
        pthread_cond_wait(&cond, &mutex);
    }
    void waitUnlock(){
        //mutex.unlock();
        pthread_mutex_unlock(&mutex);
    }

    void signal(){
        pthread_mutex_lock(&mutex);
        //mutex.lock();
        pthread_cond_signal(&cond);
        //mutex.unlock();
        pthread_mutex_unlock(&mutex);
    }

    void broadcast(){
        pthread_mutex_lock(&mutex);
        //mutex.lock();
        pthread_cond_broadcast(&cond);
        //mutex.unlock();
        pthread_mutex_unlock(&mutex);
    }

    pthread_cond_t *getHandle(){
        return &cond;
    }
    pthread_mutex_t *getMutex(){
        return &mutex;
    }

private:
    pthread_mutex_t mutex;
    //ZMutex mutex;
    pthread_cond_t cond;
};

}

#endif // ZCONDITION_H
