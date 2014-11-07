#ifndef ZCONDITION_H
#define ZCONDITION_H

#include "ztypes.h"
#include "zmutex.h"

struct _RTL_CONDITION_VARIABLE;
typedef _RTL_CONDITION_VARIABLE CONDITION_VARIABLE;

namespace LibChaos {

class ZCondition {
public:
    ZCondition();
    ~ZCondition();

    void waitOnce();

    void waitLock();
    void wait();
    void waitUnlock();

    void signal();

    void broadcast();

#ifdef ZMUTEX_WINTHREADS
    inline CONDITION_VARIABLE *getHandle(){
        return cond;
    }
    inline CRITICAL_SECTION *getMutex(){
        return mutex;
    }
#else
    inline pthread_cond_t *getHandle(){
        return &cond;
    }
    inline pthread_mutex_t *getMutex(){
        return &mutex;
    }
#endif

private:
#ifdef ZMUTEX_WINTHREADS
    CRITICAL_SECTION *mutex;
    CONDITION_VARIABLE *cond;
#else
    pthread_mutex_t mutex;
    pthread_cond_t cond;
#endif
};

}

#endif // ZCONDITION_H
