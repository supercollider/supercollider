// Copyright (C) 2001-2003
// William E. Kempf
// Copyright (C) 2007-8 Anthony Williams
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SYNC_LIBS_TEST_UTILS_HPP_INCLUDED_
#define BOOST_SYNC_LIBS_TEST_UTILS_HPP_INCLUDED_

#include <boost/config.hpp>
#include <boost/sync/condition_variables/condition_variable.hpp>
#include <boost/sync/mutexes/mutex.hpp>
#include <boost/sync/locks/unique_lock.hpp>
#include <boost/sync/support/boost_date_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#ifndef DEFAULT_EXECUTION_MONITOR_TYPE
#   define DEFAULT_EXECUTION_MONITOR_TYPE execution_monitor::use_condition
#endif

// boostinspect:nounnamed


namespace boost
{
namespace threads
{
namespace test
{
inline bool in_range(const boost::system_time& t, unsigned int secs = 1)
{
    boost::system_time now = boost::get_system_time();
    boost::posix_time::time_duration diff = t - now;
    if (diff < boost::posix_time::time_duration())
        diff = -diff;
    return diff <= boost::posix_time::seconds(secs);
}
}
}
}


namespace
{
class execution_monitor
{
    typedef boost::sync::mutex mutex_type;

public:
    enum wait_type { use_sleep_only, use_mutex, use_condition };

    execution_monitor(wait_type type, int secs) :
        done(false), type(type), secs(secs)
    {
    }

    void start()
    {
        if (type != use_sleep_only)
        {
            boost::sync::unique_lock< mutex_type > lock(mutex);
            done = false;
        }
        else
        {
            done = false;
        }
    }

    void finish()
    {
        if (type != use_sleep_only)
        {
            boost::sync::unique_lock< mutex_type > lock(mutex);
            done = true;
            if (type == use_condition)
                cond.notify_one();
        }
        else
        {
            done = true;
        }
    }

    bool wait()
    {
        boost::system_time t = boost::get_system_time() + boost::posix_time::seconds(secs);
        if (type != use_condition)
            boost::this_thread::sleep(t);
        if (type != use_sleep_only)
        {
            boost::sync::unique_lock< mutex_type > lock(mutex);
            while (type == use_condition && !done)
            {
                if (!cond.timed_wait(lock, t))
                    break;
            }
            return done;
        }
        return done;
    }

private:
    mutex_type mutex;
    boost::sync::condition_variable cond;
    bool done;
    wait_type type;
    int secs;
};

} // namespace

namespace thread_detail_anon
{
template <typename F>
class indirect_adapter
{
public:
    typedef void result_type;

    indirect_adapter(F const& func, execution_monitor& monitor) :
        func(func), monitor(monitor)
    {
    }

    void operator()() const
    {
        try
        {
            boost::thread thrd(func);
            thrd.join();
        }
        catch (...)
        {
            monitor.finish();
            throw;
        }
        monitor.finish();
    }

    BOOST_DELETED_FUNCTION(indirect_adapter& operator= (indirect_adapter const&))

private:
    F func;
    execution_monitor& monitor;
};

}
// boostinspect:nounnamed
namespace
{

template <typename F>
void timed_test(F func, int secs,
    execution_monitor::wait_type type = DEFAULT_EXECUTION_MONITOR_TYPE)
{
    execution_monitor monitor(type, secs);
    thread_detail_anon::indirect_adapter<F> ifunc(func, monitor);
    monitor.start();
    boost::thread thrd(ifunc);
    BOOST_REQUIRE_MESSAGE(monitor.wait(),
        "Timed test didn't complete in time, possible deadlock.");
}

}

namespace thread_detail_anon
{

template <typename F, typename T>
class thread_binder
{
public:
    typedef void result_type;

    thread_binder(const F& func, const T& param) :
        func(func), param(param)
    {
    }

    void operator()() const { func(param); }

private:
    F func;
    T param;
};

}

// boostinspect:nounnamed
namespace
{
template <typename F, typename T>
inline thread_detail_anon::thread_binder<F, T> bind(const F& func, const T& param)
{
    return thread_detail_anon::thread_binder<F, T>(func, param);
}
}

namespace thread_detail_anon
{

template <typename R, typename T>
class thread_member_binder
{
public:
    typedef void result_type;

    thread_member_binder(R (T::*func)(), T& param) :
        func(func), param(param)
    {
    }

    void operator()() const { (param.*func)(); }

    BOOST_DELETED_FUNCTION(thread_member_binder& operator= (thread_member_binder const&))

private:
    R (T::*func)();
    T& param;
};

}

// boostinspect:nounnamed
namespace
{
template <typename R, typename T>
inline thread_detail_anon::thread_member_binder<R, T> bind(R (T::*func)(), T& param)
{
    return thread_detail_anon::thread_member_binder<R, T>(func, param);
}
} // namespace

#endif // BOOST_SYNC_LIBS_TEST_UTILS_HPP_INCLUDED_
