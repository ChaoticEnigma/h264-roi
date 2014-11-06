#ifndef ZCONDITION_H
#define ZCONDITION_H

#include "ztypes.h"
#include "zmutex.h"

namespace LibChaos {

class ZCondition {
public:
    ZCondition(){
#ifdef ZMUTEX_WINTHREADS
        InitializeCriticalSection(&mutex);
        InitializeConditionVariable(&cond);
#else
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cond, NULL);
#endif
    }
    ~ZCondition(){
#ifdef ZMUTEX_WINTHREADS
        DeleteCriticalSection(&mutex);
#else
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
#endif
    }

    void waitOnce(){
#ifdef ZMUTEX_WINTHREADS
        EnterCriticalSection(&mutex);
        SleepConditionVariableCS(&cond, &mutex, INFINITE);
        LeaveCriticalSection(&mutex);
#else
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        pthread_mutex_unlock(&mutex);
#endif
    }

    void waitLock(){
#ifdef ZMUTEX_WINTHREADS
        EnterCriticalSection(&mutex);
#else
        pthread_mutex_lock(&mutex);
#endif
    }
    void wait(){
#ifdef ZMUTEX_WINTHREADS
        SleepConditionVariableCS(&cond, &mutex, INFINITE);
#else
        pthread_cond_wait(&cond, &mutex);
#endif
    }
    void waitUnlock(){
#ifdef ZMUTEX_WINTHREADS
        LeaveCriticalSection(&mutex);
#else
        pthread_mutex_unlock(&mutex);
#endif
    }

    void signal(){
#ifdef ZMUTEX_WINTHREADS
        EnterCriticalSection(&mutex);
        WakeConditionVariable(&cond);
        LeaveCriticalSection(&mutex);
#else
        pthread_mutex_lock(&mutex);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
#endif
    }

    void broadcast(){
#ifdef ZMUTEX_WINTHREADS
        EnterCriticalSection(&mutex);
        WakeAllConditionVariable(&cond);
        LeaveCriticalSection(&mutex);
#else
        pthread_mutex_lock(&mutex);
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
#endif
    }

#ifdef ZMUTEX_WINTHREADS
    CONDITION_VARIABLE *getHandle(){
        return &cond;
    }
    CRITICAL_SECTION *getMutex(){
        return &mutex;
    }
#else
    pthread_cond_t *getHandle(){
        return &cond;
    }
    pthread_mutex_t *getMutex(){
        return &mutex;
    }
#endif

private:
#ifdef ZMUTEX_WINTHREADS
    CRITICAL_SECTION mutex;
    CONDITION_VARIABLE cond;
#else
    pthread_mutex_t mutex;
    pthread_cond_t cond;
#endif
};

}

#endif // ZCONDITION_H
