#pragma once
#include <mutex> //unique_lock
// Xenomai enforces the requirement to hold the lock so that optimizing
// the wake up process is possible - under the assumption that the caller
// owns the lock. There is no hack around this.
// See: https://www.xenomai.org/pipermail/xenomai/2017-October/037759.html
#define SC_CONDITION_VARIABLE_ANY_SHOULD_LOCK_BEFORE_NOTIFY

// to make sure Xenomai gets initialised on time, create one object of this
// class before any XenomaiMutex or XenomaiConditionVariable constructors
class XenomaiInitializer {
public:
    XenomaiInitializer();
};

class XenomaiMutex {
    friend class XenomaiConditionVariable;

public:
    XenomaiMutex();
    ~XenomaiMutex();

    bool try_lock();
    void lock();
    void unlock();

private:
    pthread_mutex_t m_mutex;
    bool m_enabled = false;
};

class XenomaiConditionVariable {
public:
    XenomaiConditionVariable();
    ~XenomaiConditionVariable();

    void wait(std::unique_lock<XenomaiMutex>& lck);
    void notify_one() noexcept;
    void notify_all() noexcept;

private:
    pthread_cond_t m_cond;
    bool m_enabled = false;
};
