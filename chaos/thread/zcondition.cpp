#include "zcondition.h"
#include "zerror.h"

#ifdef ZMUTEX_WINTHREADS
    #include <windows.h>
#else
    #include <pthread.h>
#endif

namespace LibChaos {

#ifdef ZMUTEX_WINTHREADS
ZCondition::ZCondition() : mutex(new CRITICAL_SECTION), cond(new CONDITION_VARIABLE){
    InitializeCriticalSection(mutex);
    InitializeConditionVariable(cond);
}
#else
ZCondition::ZCondition(){
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}
#endif

ZCondition::ZCondition(const ZCondition &){
    throw ZError("Someone tried to copy a ZCondition");
}

ZCondition::~ZCondition(){
#ifdef ZMUTEX_WINTHREADS
    DeleteCriticalSection(mutex);
#else
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
#endif
}

void ZCondition::waitOnce(){
#ifdef ZMUTEX_WINTHREADS
    EnterCriticalSection(mutex);
    SleepConditionVariableCS(cond, mutex, INFINITE);
    LeaveCriticalSection(mutex);
#else
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
#endif
}

void ZCondition::waitLock(){
#ifdef ZMUTEX_WINTHREADS
    EnterCriticalSection(mutex);
#else
    pthread_mutex_lock(&mutex);
#endif
}

void ZCondition::wait(){
#ifdef ZMUTEX_WINTHREADS
    SleepConditionVariableCS(cond, mutex, INFINITE);
#else
    pthread_cond_wait(&cond, &mutex);
#endif
}

void ZCondition::waitUnlock(){
#ifdef ZMUTEX_WINTHREADS
    LeaveCriticalSection(mutex);
#else
    pthread_mutex_unlock(&mutex);
#endif
}

void ZCondition::signal(){
#ifdef ZMUTEX_WINTHREADS
    EnterCriticalSection(mutex);
    WakeConditionVariable(cond);
    LeaveCriticalSection(mutex);
#else
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
#endif
}

void ZCondition::broadcast(){
#ifdef ZMUTEX_WINTHREADS
    EnterCriticalSection(mutex);
    WakeAllConditionVariable(cond);
    LeaveCriticalSection(mutex);
#else
    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
#endif
}



}
