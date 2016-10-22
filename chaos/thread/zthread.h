/*******************************************************************************
**                                  LibChaos                                  **
**                                  zthread.h                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZTHREAD_H
#define ZTHREAD_H

#include "ztypes.h"
#include "zmutex.h"

#include <atomic>

#if PLATFORM == WINDOWS || PLATFORM == CYGWIN
    #define ZTHREAD_WINTHREADS
#endif

#ifndef ZTHREAD_WINTHREADS
    #include <pthread.h>
#endif

#undef bool // This is a thing in standard headers?

namespace LibChaos {

/*! Cross-platform thread handle abstraction.
 *  \ingroup Thread
 */
class ZThread {
public:
    struct zthreadparam;

    struct ZThreadArg {
        void *arg;
        std::atomic<bool> stop;
        ZThread *thread;
        zthreadparam param;
    };

    typedef zu64 ztid;
    typedef void *(*funcType)(ZThreadArg);

public:
    //! Create thread and run \a func with \a user data.
    ZThread(funcType func = nullptr, void *arg = nullptr);

    // Delete copy constructor
    ZThread(const ZThread &other) = delete;
    // Delete assignment operator
    ZThread &operator=(const ZThread &other) = delete;

    //! Destroying thread container detaches the thread first.
    ~ZThread();

    //! Start thread.
    bool exec(void *user = nullptr);

    //! Block the current thread until the thread finishes, return its return value.
    void *join();

    /*! Kill the thread as soon as possible.
     *  When exactly the thread stops is up to the OS.
     *  On POSIX systems, the thread will be cancelled at the next cancellation point,
     *  which are system calls like sleep().
     */
    void kill();

    //! Request graceful thread exit.
    void stop();

    /*! Detach thread handle.
     *  Clear this thread object.
     *  \warning Detached threads cannot be joined or re-attached.
     */
    void detach();

    //! Check id the thread is still alive.
    bool alive();

    //! Get the thread id of the thread.
    ztid tid();
    //! Get the thread id of the current thread.
    static ztid thisTid();

    //! Yield the current thread to other threads.
    static void yield();

    //! Sleep the current thread for \a seconds.
    static void sleep(zu32 seconds);
    //! Sleep the current thread for \a milliseconds.
    static void msleep(zu32 milliseconds);
    //! Sleep the current thread for \a microseconds.
    static void usleep(zu32 microseconds);

private:
    /*! Overloadable thread run method.
     *  The default method calls the external callback.
     */
    virtual void *run(ZThreadArg arg);

private:
    //! Common thread entry point.
    void *_entry_common();

#ifdef ZTHREAD_WINTHREADS
#if COMPILER == MSVC
    static DWORD _stdcall _entry_win(LPVOID ptr);
#else
    static DWORD _entry_win(LPVOID ptr);
#endif
#else
    static void *_entry_posix(void *ptr);
#endif

private:
    struct zthreadparam {
        ZThread *handle;
        ZMutex mutex;
        std::atomic<bool> stop;
    };

private:
#ifdef ZTHREAD_WINTHREADS
    typedef void *HANDLE;
    HANDLE _thread;
#else
    pthread_t _thread;
#endif

    funcType _userfunc;
    void *_userarg;

    zthreadparam *_param;
    std::atomic<bool> _alive;
};

} // namespace LibChaos

#endif // ZTHREAD_H
