// Copyright (C) 2007 Anthony Williams
// Copyright (C) 2013 Andrey Semashev
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/test/unit_test.hpp>
#include "utils.hpp"
#include "condition_test_common.hpp"

unsigned const number_of_test_threads = 5;

void do_test_condition_notify_all_wakes_from_wait()
{
    wait_for_flag data;

    boost::thread_group group;

    try
    {
        for(unsigned i=0;i<number_of_test_threads;++i)
        {
            group.create_thread(bind(&wait_for_flag::wait_without_predicate, data));
        }

        {
            boost::sync::unique_lock<boost::sync::mutex> lock(data.mutex);
            data.flag=true;
            data.cond_var.notify_all();
        }

        group.join_all();
        BOOST_CHECK_EQUAL(data.woken,number_of_test_threads);
    }
    catch(...)
    {
        group.join_all();
        throw;
    }
}

BOOST_AUTO_TEST_CASE(test_condition_notify_all_wakes_from_wait)
{
    timed_test(&do_test_condition_notify_all_wakes_from_wait, timeout_seconds);
}

void do_test_condition_notify_all_wakes_from_wait_with_predicate()
{
    wait_for_flag data;

    boost::thread_group group;

    try
    {
        for(unsigned i=0;i<number_of_test_threads;++i)
        {
            group.create_thread(bind(&wait_for_flag::wait_with_predicate, data));
        }

        {
            boost::sync::unique_lock<boost::sync::mutex> lock(data.mutex);
            data.flag=true;
            data.cond_var.notify_all();
        }

        group.join_all();
        BOOST_CHECK_EQUAL(data.woken,number_of_test_threads);
    }
    catch(...)
    {
        group.join_all();
        throw;
    }
}

BOOST_AUTO_TEST_CASE(test_condition_notify_all_wakes_from_wait_with_predicate)
{
    timed_test(&do_test_condition_notify_all_wakes_from_wait_with_predicate, timeout_seconds);
}

void do_test_condition_notify_all_wakes_from_timed_wait()
{
    wait_for_flag data;

    boost::thread_group group;

    try
    {
        for(unsigned i=0;i<number_of_test_threads;++i)
        {
            group.create_thread(bind(&wait_for_flag::timed_wait_without_predicate, data));
        }

        {
            boost::sync::unique_lock<boost::sync::mutex> lock(data.mutex);
            data.flag=true;
            data.cond_var.notify_all();
        }

        group.join_all();
        BOOST_CHECK_EQUAL(data.woken,number_of_test_threads);
    }
    catch(...)
    {
        group.join_all();
        throw;
    }
}

BOOST_AUTO_TEST_CASE(test_condition_notify_all_wakes_from_timed_wait)
{
    timed_test(&do_test_condition_notify_all_wakes_from_timed_wait, timeout_seconds);
}

void do_test_condition_notify_all_wakes_from_timed_wait_with_predicate()
{
    wait_for_flag data;

    boost::thread_group group;

    try
    {
        for(unsigned i=0;i<number_of_test_threads;++i)
        {
            group.create_thread(bind(&wait_for_flag::timed_wait_with_predicate, data));
        }

        {
            boost::sync::unique_lock<boost::sync::mutex> lock(data.mutex);
            data.flag=true;
            data.cond_var.notify_all();
        }

        group.join_all();
        BOOST_CHECK_EQUAL(data.woken,number_of_test_threads);
    }
    catch(...)
    {
        group.join_all();
        throw;
    }
}

BOOST_AUTO_TEST_CASE(test_condition_notify_all_wakes_from_timed_wait_with_predicate)
{
    timed_test(&do_test_condition_notify_all_wakes_from_timed_wait_with_predicate, timeout_seconds);
}

void do_test_condition_notify_all_wakes_from_relative_timed_wait_with_predicate()
{
    wait_for_flag data;

    boost::thread_group group;

    try
    {
        for(unsigned i=0;i<number_of_test_threads;++i)
        {
            group.create_thread(bind(&wait_for_flag::relative_timed_wait_with_predicate, data));
        }

        {
            boost::sync::unique_lock<boost::sync::mutex> lock(data.mutex);
            data.flag=true;
            data.cond_var.notify_all();
        }

        group.join_all();
        BOOST_CHECK_EQUAL(data.woken,number_of_test_threads);
    }
    catch(...)
    {
        group.join_all();
        throw;
    }
}

BOOST_AUTO_TEST_CASE(test_condition_notify_all_wakes_from_relative_timed_wait_with_predicate)
{
    timed_test(&do_test_condition_notify_all_wakes_from_relative_timed_wait_with_predicate, timeout_seconds);
}

namespace {

boost::sync::mutex multiple_wake_mutex;
boost::sync::condition_variable multiple_wake_cond;
unsigned multiple_wake_count = 0;

void wait_for_condvar_and_increase_count()
{
    boost::sync::unique_lock<boost::sync::mutex> lk(multiple_wake_mutex);
    multiple_wake_cond.wait(lk);
    ++multiple_wake_count;
}

} // namespace

void do_test_notify_all_following_notify_one_wakes_all_threads()
{
    boost::thread thread1(wait_for_condvar_and_increase_count);
    boost::thread thread2(wait_for_condvar_and_increase_count);

    boost::this_thread::sleep(boost::posix_time::milliseconds(200));
    multiple_wake_cond.notify_one();

    boost::thread thread3(wait_for_condvar_and_increase_count);

    boost::this_thread::sleep(boost::posix_time::milliseconds(200));
    multiple_wake_cond.notify_one();
    multiple_wake_cond.notify_all();
    boost::this_thread::sleep(boost::posix_time::milliseconds(200));

    {
        boost::sync::unique_lock<boost::sync::mutex> lk(multiple_wake_mutex);
        BOOST_CHECK(multiple_wake_count==3);
    }

    thread1.join();
    thread2.join();
    thread3.join();
}

BOOST_AUTO_TEST_CASE(test_notify_all_following_notify_one_wakes_all_threads)
{
    timed_test(&do_test_notify_all_following_notify_one_wakes_all_threads, timeout_seconds);
}
