#include "XenomaiLock.h"

#include <pthread.h>
#include <error.h>
#include <string.h>
#include <cobalt/sys/cobalt.h>
#include <xenomai/init.h>
#ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#endif
#include <unistd.h>
#include <sys/syscall.h>

#ifndef __COBALT__
#    error This should be compiled with __COBALT__ and the appropriate Xenomai headers in the #include path
#endif // __COBALT__
// All __wrap_* calls below are Xenomai libcobalt calls

//#define PRINT_XENO_LOCK

#ifdef PRINT_XENO_LOCK
#    define xprintf printf
#    define xfprintf fprintf
#else // not PRINT_XENO_LOCK
#    define xprintf(...)
#    define xfprintf(...)
#endif // PRINT_XENO_LOCK

// Standard Linux `gettid(2)` not available on Bela
static inline pid_t getTid() {
    pid_t tid = syscall(SYS_gettid);
    return tid;
}

// throughout, we use heuristics to check whether Xenomai needs to be
// initialised and whether the current thread is a Xenomai thread.
// See https://www.xenomai.org/pipermail/xenomai/2019-January/040203.html
static void initializeXenomai() {
    xprintf("initializeXenomai\n");
    int argc = 2;
    char blankOpt[] = "";
#ifdef PRINT_XENO_LOCK
    char traceOpt[] = "--trace";
#else // not PRINT_XENO_LOCK
    char traceOpt[] = "";
#endif // PRINT_XENO_LOCK

    char* const argv[argc] = { blankOpt, traceOpt };
    char* const* argvPtrs[argc] = { &argv[0], &argv[1] };
    xenomai_init(&argc, argvPtrs);
}

static bool turnIntoCobaltThread(bool recurred = false) {
    struct sched_param param;
    memset(&param, 0, sizeof(param));
    int policy;
    // Guaranteed to succeed as pthread_self() cannot fail and pthread_getschedparam()'s only error condition is when
    // the given thread does not exist.
    pthread_getschedparam(pthread_self(), &policy, &param);
    pid_t tid = getTid();

    if (int ret = __wrap_sched_setscheduler(tid, policy, &param)) {
        fprintf(stderr, "Warning: unable to turn current thread into a Xenomai thread : (%d) %s\n", -ret,
                strerror(-ret));
        initializeXenomai();
        if (!recurred)
            return turnIntoCobaltThread(true);
        else
            return false;
    }
    xprintf("Turned thread %d into a Cobalt thread %s\n", tid, recurred ? "with recursion" : "");
    return true;
}

XenomaiInitializer::XenomaiInitializer() { initializeXenomai(); }

XenomaiMutex::XenomaiMutex() {
    xprintf("Construct mutex\n");
    if (int ret = __wrap_pthread_mutex_init(&m_mutex, NULL)) {
        if (EPERM != ret) {
            xprintf("__wrap_thread_mutex_init failed with %d %s\n", ret, strerror(ret));
            return;
        } else {
            xprintf("mutex init returned EPERM\n");
            initializeXenomai();
            if (int ret = __wrap_pthread_mutex_init(&m_mutex, NULL)) {
                fprintf(stderr, "Error: unable to initialize mutex : (%d) %s\n", ret, strerror(-ret));
                return;
            }
        }
    }
    m_enabled = true;
}

XenomaiMutex::~XenomaiMutex() {
    xprintf("Destroy mutex %p\n", &m_mutex);
    if (m_enabled)
        __wrap_pthread_mutex_destroy(&m_mutex);
}

// a helper function to try
// - call func()
// - if it fails, try to turn the current thread into a cobalt thread and call func() again
// - if it fails again, just fail
// - return true if it succeeds, or false if it fails
// id and name are just for debugging purposes, while m_enabled is there because it saves duplicating some lines
template <typename F, typename T> static bool tryOrRetryImpl(F&& func, bool m_enabled, T* id, const char* name) {
    xprintf("tid: %d ", getTid());
    if (!m_enabled) {
        xfprintf(stderr, "%s disabled %p\n", name, id);
        return false;
    }

    xprintf("%s %p\n", name, id);

    int ret = func();
    // 0 is "success" (or at least meaningful failure)
    if (0 == ret) {
        return true;
    } else if (EPERM != ret) {
        return false;
    } else {
        // if we got EPERM, we are not a Xenomai thread
        if (!turnIntoCobaltThread()) {
            xfprintf(stderr, "%s %p could not turn into cobalt\n", name, id);
            return false;
        }
    }

    // retry after becoming a cobalt thread
    ret = func();
    if (0 == ret) {
        return true;
    } else {
        xfprintf(stderr, "%s %p failed after having turned into cobalt: %d\n", name, id, ret);
        return false;
    }
}

// Helper macro to insert this-ptr and function name
#define tryOrRetry(_func_, _enabled_) tryOrRetryImpl(_func_, _enabled_, this, __func__)

// condition resource_deadlock_would_occur instead of deadlocking. https://en.cppreference.com/w/cpp/thread/mutex/lock
bool XenomaiMutex::try_lock() {
    return tryOrRetry([this]() { return __wrap_pthread_mutex_trylock(&this->m_mutex); }, m_enabled);
    // TODO: An implementation that can detect the invalid usage is encouraged to throw a std::system_error with error
    // condition resource_deadlock_would_occur instead of deadlocking.
}

void XenomaiMutex::lock() {
    tryOrRetry([this]() { return __wrap_pthread_mutex_lock(&this->m_mutex); }, m_enabled);
}

void XenomaiMutex::unlock() {
    tryOrRetry([this]() { return __wrap_pthread_mutex_unlock(&this->m_mutex); }, m_enabled);
}

XenomaiConditionVariable::XenomaiConditionVariable() {
    xprintf("Construct CondictionVariable\n");
    if (int ret = __wrap_pthread_cond_init(&m_cond, NULL)) {
        if (EPERM != ret) {
            xprintf("__wrap_thread_cond_init failed with %d %s\n", ret, strerror(ret));
            return;
        } else {
            xprintf("mutex init returned EPERM\n");
            initializeXenomai();
            if (int ret = __wrap_pthread_cond_init(&m_cond, NULL)) {
                fprintf(stderr, "Error: unable to create condition variable : (%d) %s\n", ret, strerror(ret));
                return;
            }
        }
    }
    m_enabled = true;
}

XenomaiConditionVariable::~XenomaiConditionVariable() {
    if (m_enabled) {
        notify_all();
        __wrap_pthread_cond_destroy(&m_cond);
    }
}

void XenomaiConditionVariable::wait(std::unique_lock<XenomaiMutex>& lck) {
    // If any parameter has a value that is not valid for this function (such as if lck's mutex object is not locked by
    // the calling thread), it causes undefined behavior.

    // Otherwise, if an exception is thrown, both the condition_variable_any object and the arguments are in a valid
    // state (basic guarantee). Additionally, on exception, the state of lck is attempted to be restored before exiting
    // the function scope (by calling lck.lock()).

    // It may throw system_error in case of failure (transmitting any error condition from the respective call to lock
    // or unlock). The predicate version (2) may also throw exceptions thrown by pred.
    tryOrRetry(([this, &lck]() { return __wrap_pthread_cond_wait(&this->m_cond, &lck.mutex()->m_mutex); }), m_enabled);
}

void XenomaiConditionVariable::notify_one() noexcept {
    tryOrRetry([this]() { return __wrap_pthread_cond_signal(&this->m_cond); }, m_enabled);
}

void XenomaiConditionVariable::notify_all() noexcept {
    tryOrRetry([this]() { return __wrap_pthread_cond_broadcast(&this->m_cond); }, m_enabled);
}
