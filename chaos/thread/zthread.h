/*******************************************************************************
**                                  LibChaos                                  **
**                                  zthread.h                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZTHREAD_H
#define ZTHREAD_H

#include "ztypes.h"
#include "zmutex.h"

#if PLATFORM == WINDOWS || PLATFORM == CYGWIN
    #define ZTHREAD_WINTHREADS
#endif

#ifndef ZTHREAD_WINTHREADS
    #include <pthread.h>
#endif

#include <atomic>

#undef bool // This is a thing in standard headers?

namespace LibChaos {

typedef zu64 ztid;

struct ZThreadArg {
    bool stop();
    void *arg;
    std::atomic<bool> _stop;
};

/*! Cross-platform thread handle abstraction.
 *  \ingroup Thread
 */
class ZThread {
public:
    typedef void *(*funcType)(ZThreadArg *);

    struct zthreadparam {
        ZThread *handle;
        funcType func;
        ZThreadArg zarg;
        ZMutex mutex;
    };

public:
    //! Create empty thread container.
    ZThread();

    //! Create thread and run \a func.
    ZThread(funcType func);
    //! Create thread and run \a func with \a user data.
    ZThread(funcType func, void *user);

    // Delete copy constructor
    ZThread(const ZThread &other) = delete;
    // Delete assignment operator
    ZThread &operator=(const ZThread &other) = delete;

    ~ZThread();

    //! Start thread and run \a func.
    bool run(funcType func);
    //! Start thread and run \a func with \a user data.
    bool run(funcType func, void *user);

    //! Block the current thread until the thread finishes, return its return value.
    void *join();

    //! Kill the thread as soon as possible, when exactly is up to the OS.
    void kill();

    //! Request thread graceful exit.
    void stop();

    /*! Detach thread handle.
     *  Clear this thread object.
     *  \warning Detached threads cannot be joined or re-attached.
     */
    void detach();

    //! Yield the current thread to other threads.
    static void yield();

    //! Sleep the current thread for \a seconds.
    static void sleep(zu32 seconds);
    //! Sleep the current thread for \a milliseconds.
    static void msleep(zu32 milliseconds);
    //! Sleep the current thread for \a microseconds.
    static void usleep(zu32 microseconds);

    //! Get the thread id of the thread.
    ztid tid();
    //! Get the thread id of the current thread.
    static ztid thisTid();

    //! Check id the thread is still alive.
    bool alive();

public:
    void _setThreadAlive(bool alive);

private:
#ifdef ZTHREAD_WINTHREADS
    typedef void *HANDLE;
    HANDLE _thread;
#else
    pthread_t _thread;
#endif

    zthreadparam *_param;
    std::atomic<bool> _alive;
    bool _copyable;
};

} // namespace LibChaos

#endif // ZTHREAD_H
