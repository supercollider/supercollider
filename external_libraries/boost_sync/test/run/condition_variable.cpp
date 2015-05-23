// Copyright (C) 2001-2003
// William E. Kempf
// Copyright (C) 2007 Anthony Williams
// Copyright (C) 2013 Andrey Semashev
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config.hpp>
#include <boost/bind.hpp>
#include <boost/sync/locks/unique_lock.hpp>
#include <boost/sync/mutexes/mutex.hpp>
#include <boost/sync/condition_variables/condition_variable.hpp>
#include <boost/sync/support/boost_date_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/test/unit_test.hpp>
#include "utils.hpp"

struct condition_test_data
{
    condition_test_data() : notified(0), awoken(0) { }

    boost::sync::mutex mutex;
    boost::sync::condition_variable condition;
    int notified;
    int awoken;
};

void condition_test_thread(condition_test_data* data)
{
    boost::sync::unique_lock<boost::sync::mutex> lock(data->mutex);
    BOOST_CHECK(lock ? true : false);
    while (!(data->notified > 0))
        data->condition.wait(lock);
    BOOST_CHECK(lock ? true : false);
    data->awoken++;
}

struct cond_predicate
{
    cond_predicate(int& var, int val) : _var(var), _val(val) { }

    bool operator()() { return _var == _val; }

    int& _var;
    int _val;

    BOOST_DELETED_FUNCTION(cond_predicate& operator=(cond_predicate const&))
};

void condition_test_waits(condition_test_data* data)
{
    boost::sync::unique_lock<boost::sync::mutex> lock(data->mutex);
    BOOST_CHECK(lock ? true : false);

    // Test wait.
    while (data->notified != 1)
        data->condition.wait(lock);
    BOOST_CHECK(lock ? true : false);
    BOOST_CHECK_EQUAL(data->notified, 1);
    data->awoken++;
    data->condition.notify_one();

    // Test predicate wait.
    data->condition.wait(lock, cond_predicate(data->notified, 2));
    BOOST_CHECK(lock ? true : false);
    BOOST_CHECK_EQUAL(data->notified, 2);
    data->awoken++;
    data->condition.notify_one();

    // Test timed_wait.
    boost::system_time xt = boost::get_system_time() + boost::posix_time::seconds(10);
    while (data->notified != 3)
        data->condition.timed_wait(lock, xt);
    BOOST_CHECK(lock ? true : false);
    BOOST_CHECK_EQUAL(data->notified, 3);
    data->awoken++;
    data->condition.notify_one();

    // Test predicate timed_wait.
    xt = boost::get_system_time() + boost::posix_time::seconds(10);
    cond_predicate pred(data->notified, 4);
    BOOST_CHECK(data->condition.timed_wait(lock, xt, pred));
    BOOST_CHECK(lock ? true : false);
    BOOST_CHECK(pred());
    BOOST_CHECK_EQUAL(data->notified, 4);
    data->awoken++;
    data->condition.notify_one();

    // Test predicate timed_wait with relative timeout
    cond_predicate pred_rel(data->notified, 5);
    BOOST_CHECK(data->condition.timed_wait(lock, boost::posix_time::seconds(10), pred_rel));
    BOOST_CHECK(lock ? true : false);
    BOOST_CHECK(pred_rel());
    BOOST_CHECK_EQUAL(data->notified, 5);
    data->awoken++;
    data->condition.notify_one();
}

void do_test_condition_waits()
{
    condition_test_data data;

    boost::thread thread(boost::bind(&condition_test_waits, &data));

    {
        boost::sync::unique_lock<boost::sync::mutex> lock(data.mutex);
        BOOST_CHECK(lock ? true : false);

        boost::thread::sleep(boost::get_system_time() + boost::posix_time::seconds(1));
        data.notified++;
        data.condition.notify_one();
        while (data.awoken != 1)
            data.condition.wait(lock);
        BOOST_CHECK(lock ? true : false);
        BOOST_CHECK_EQUAL(data.awoken, 1);

        boost::thread::sleep(boost::get_system_time() + boost::posix_time::seconds(1));
        data.notified++;
        data.condition.notify_one();
        while (data.awoken != 2)
            data.condition.wait(lock);
        BOOST_CHECK(lock ? true : false);
        BOOST_CHECK_EQUAL(data.awoken, 2);

        boost::thread::sleep(boost::get_system_time() + boost::posix_time::seconds(1));
        data.notified++;
        data.condition.notify_one();
        while (data.awoken != 3)
            data.condition.wait(lock);
        BOOST_CHECK(lock ? true : false);
        BOOST_CHECK_EQUAL(data.awoken, 3);

        boost::thread::sleep(boost::get_system_time() + boost::posix_time::seconds(1));
        data.notified++;
        data.condition.notify_one();
        while (data.awoken != 4)
            data.condition.wait(lock);
        BOOST_CHECK(lock ? true : false);
        BOOST_CHECK_EQUAL(data.awoken, 4);


        boost::thread::sleep(boost::get_system_time() + boost::posix_time::seconds(1));
        data.notified++;
        data.condition.notify_one();
        while (data.awoken != 5)
            data.condition.wait(lock);
        BOOST_CHECK(lock ? true : false);
        BOOST_CHECK_EQUAL(data.awoken, 5);
    }

    thread.join();
    BOOST_CHECK_EQUAL(data.awoken, 5);
}

BOOST_AUTO_TEST_CASE(test_condition_waits)
{
    // We should have already tested notify_one here, so
    // a timed test with the default execution_monitor::use_condition
    // should be OK, and gives the fastest performance
    timed_test(&do_test_condition_waits, 12);
}
