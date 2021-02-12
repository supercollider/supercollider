// Copyright (C) 2001-2003
// William E. Kempf
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/core/lightweight_test.hpp>

#include <boost/bind.hpp>
#include <boost/mpl/if.hpp>
#include <boost/sync/mutexes.hpp>
#include <boost/sync/locks.hpp>
#include <boost/sync/condition_variables/condition_variable.hpp>
#include <boost/sync/condition_variables/condition_variable_any.hpp>
#include <boost/sync/traits/is_condition_variable_compatible.hpp>
#include <boost/sync/support/boost_date_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "utils.hpp"

template <typename M>
struct test_lock
{
    typedef M mutex_type;
    typedef typename boost::mpl::if_<
        boost::sync::is_condition_variable_compatible< mutex_type >,
        boost::sync::condition_variable,
        boost::sync::condition_variable_any
    >::type condition_variable_type;
    typedef boost::sync::unique_lock< mutex_type > lock_type;

    void operator()() const
    {
        mutex_type mutex;
        condition_variable_type condition;

        // Test the lock's constructors.
        {
            lock_type lock(mutex, boost::sync::defer_lock);
            BOOST_TEST(!lock);
        }
        lock_type lock(mutex);
        BOOST_TEST(lock ? true : false);

        // Construct and initialize an xtime for a fast time out.
        boost::system_time timeout = boost::get_system_time() + boost::posix_time::milliseconds(100);

        // Test the lock and the mutex with condition variables.
        // No one is going to notify this condition variable.  We expect to
        // time out.
        BOOST_TEST(!condition.timed_wait(lock, timeout));
        BOOST_TEST(lock ? true : false);

        // Test the lock and unlock methods.
        lock.unlock();
        BOOST_TEST(!lock);
        lock.lock();
        BOOST_TEST(lock ? true : false);
    }
};

template <typename M>
struct test_trylock
{
    typedef M mutex_type;
    typedef typename boost::mpl::if_<
        boost::sync::is_condition_variable_compatible< mutex_type >,
        boost::sync::condition_variable,
        boost::sync::condition_variable_any
    >::type condition_variable_type;
    typedef boost::sync::unique_lock< mutex_type > lock_type;

    void operator()()
    {
        mutex_type mutex;
        condition_variable_type condition;

        // Test the lock's constructors.
        {
            lock_type lock(mutex, boost::sync::try_to_lock);
            BOOST_TEST(lock ? true : false);
        }
        {
            lock_type lock(mutex, boost::sync::defer_lock);
            BOOST_TEST(!lock);
        }
        lock_type lock(mutex, boost::sync::try_to_lock);
        BOOST_TEST(lock ? true : false);

        // Construct and initialize an xtime for a fast time out.
        boost::system_time timeout = boost::get_system_time() + boost::posix_time::milliseconds(100);

        // Test the lock and the mutex with condition variables.
        // No one is going to notify this condition variable.  We expect to
        // time out.
        BOOST_TEST(!condition.timed_wait(lock, timeout));
        BOOST_TEST(lock ? true : false);

        // Test the lock, unlock and trylock methods.
        lock.unlock();
        BOOST_TEST(!lock);
        lock.lock();
        BOOST_TEST(lock ? true : false);
        lock.unlock();
        BOOST_TEST(!lock);
        BOOST_TEST(lock.try_lock());
        BOOST_TEST(lock ? true : false);
    }
};

template<typename Mutex>
struct test_lock_times_out_if_other_thread_has_lock
{
    typedef boost::sync::unique_lock<Mutex> Lock;
    typedef boost::sync::mutex done_mutex_type;

    Mutex m;
    done_mutex_type done_mutex;
    bool done;
    bool locked;
    boost::sync::condition_variable done_cond;

    test_lock_times_out_if_other_thread_has_lock() :
        done(false),locked(false)
    {
    }

    void locking_thread()
    {
        Lock lock(m, boost::sync::defer_lock);
        lock.timed_lock(boost::posix_time::milliseconds(50));

        boost::sync::lock_guard< done_mutex_type > lk(done_mutex);
        locked = lock.owns_lock();
        done = true;
        done_cond.notify_one();
    }

    void locking_thread_through_constructor()
    {
        Lock lock(m, boost::posix_time::milliseconds(50));

        boost::sync::lock_guard< done_mutex_type > lk(done_mutex);
        locked = lock.owns_lock();
        done = true;
        done_cond.notify_one();
    }

    bool is_done() const
    {
        return done;
    }

    typedef test_lock_times_out_if_other_thread_has_lock<Mutex> this_type;

    void do_test(void (this_type::*test_func)())
    {
        Lock lock(m);

        locked = false;
        done = false;

        boost::thread t(test_func, this);

        try
        {
            {
                boost::sync::unique_lock< done_mutex_type > lk(done_mutex);
                BOOST_TEST(done_cond.timed_wait(lk, boost::posix_time::seconds(2), boost::bind(&this_type::is_done, this)));
                BOOST_TEST(!locked);
            }

            lock.unlock();
            t.join();
        }
        catch (...)
        {
            lock.unlock();
            t.join();
            throw;
        }
    }


    void operator()()
    {
        do_test(&this_type::locking_thread);
        do_test(&this_type::locking_thread_through_constructor);
    }
};

template <typename M>
struct test_timedlock
{
    typedef M mutex_type;
    typedef typename boost::mpl::if_<
        boost::sync::is_condition_variable_compatible< mutex_type >,
        boost::sync::condition_variable,
        boost::sync::condition_variable_any
    >::type condition_variable_type;
    typedef boost::sync::unique_lock< mutex_type > lock_type;
    typedef boost::sync::unique_lock< mutex_type > timed_lock_type;

    static bool fake_predicate()
    {
        return false;
    }

    void operator()()
    {
        test_lock_times_out_if_other_thread_has_lock<mutex_type>()();

        mutex_type mutex;
        condition_variable_type condition;

        // Test the lock's constructors.
        {
            // Construct and initialize an xtime for a fast time out.
            boost::system_time xt = boost::get_system_time() + boost::posix_time::milliseconds(100);

            timed_lock_type lock(mutex, xt);
            BOOST_TEST(lock ? true : false);
        }
        {
            timed_lock_type lock(mutex, boost::sync::defer_lock);
            BOOST_TEST(!lock);
        }
        timed_lock_type lock(mutex);
        BOOST_TEST(lock ? true : false);

        // Construct and initialize an xtime for a fast time out.
        boost::system_time timeout = boost::get_system_time() + boost::posix_time::milliseconds(100);

        // Test the lock and the mutex with condition variables.
        // No one is going to notify this condition variable.  We expect to
        // time out.
        BOOST_TEST(!condition.timed_wait(lock, timeout, fake_predicate));
        BOOST_TEST(lock ? true : false);

        boost::system_time now = boost::get_system_time();
        boost::posix_time::milliseconds const timeout_resolution(20);
        BOOST_TEST((timeout - timeout_resolution) < now);

        // Test the lock, unlock and timedlock methods.
        lock.unlock();
        BOOST_TEST(!lock);
        lock.lock();
        BOOST_TEST(lock ? true : false);
        lock.unlock();
        BOOST_TEST(!lock);
        boost::system_time target = boost::get_system_time() + boost::posix_time::milliseconds(100);
        BOOST_TEST(lock.timed_lock(target));
        BOOST_TEST(lock ? true : false);
        lock.unlock();
        BOOST_TEST(!lock);

        BOOST_TEST(mutex.timed_lock(boost::posix_time::milliseconds(100)));
        mutex.unlock();

        BOOST_TEST(lock.timed_lock(boost::posix_time::milliseconds(100)));
        BOOST_TEST(lock ? true : false);
        lock.unlock();
        BOOST_TEST(!lock);
    }
};

template <typename M>
struct test_recursive_lock
{
    typedef M mutex_type;
    typedef boost::sync::unique_lock< mutex_type > lock_type;

    void operator()()
    {
        mutex_type mx;
        lock_type lock1(mx);
        lock_type lock2(mx);
    }
};



void do_test_mutex()
{
    test_lock<boost::sync::mutex>()();
}

void do_test_spin_mutex()
{
    test_lock<boost::sync::spin_mutex>()();
}

void do_test_shared_spin_mutex()
{
    test_lock<boost::sync::shared_spin_mutex>()();
}

void test_mutex()
{
    timed_test(&do_test_mutex, 3);
    timed_test(&do_test_spin_mutex, 3);
}

void do_test_try_mutex()
{
    test_trylock<boost::sync::mutex>()();
}

void test_try_mutex()
{
    timed_test(&do_test_try_mutex, 3);
}


void do_test_timed_mutex()
{
    test_lock<boost::sync::timed_mutex>()();
    test_trylock<boost::sync::timed_mutex>()();
    test_timedlock<boost::sync::timed_mutex>()();
}

void test_timed_mutex()
{
    timed_test(&do_test_timed_mutex, 3);
}

/*
void do_test_recursive_mutex()
{
    test_lock<boost::recursive_mutex>()();
    test_recursive_lock<boost::recursive_mutex>()();
}

void test_recursive_mutex()
{
    timed_test(&do_test_recursive_mutex, 3);
}

void do_test_recursive_try_mutex()
{
    test_lock<boost::recursive_try_mutex>()();
    test_trylock<boost::recursive_try_mutex>()();
    test_recursive_lock<boost::recursive_try_mutex>()();
}

void test_recursive_try_mutex()
{
    timed_test(&do_test_recursive_try_mutex, 3);
}

void do_test_recursive_timed_mutex()
{
    test_lock<boost::recursive_timed_mutex>()();
    test_trylock<boost::recursive_timed_mutex>()();
    test_timedlock<boost::recursive_timed_mutex>()();
    test_recursive_lock<boost::recursive_timed_mutex>()();
}

void test_recursive_timed_mutex()
{
    timed_test(&do_test_recursive_timed_mutex, 3);
}
*/

int main()
{
    test_mutex();
    test_try_mutex();
    test_timed_mutex();

    return boost::report_errors();
}
