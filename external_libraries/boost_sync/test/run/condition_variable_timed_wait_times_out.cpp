// Copyright (C) 2007-8 Anthony Williams
// Copyright (C) 2013 Andrey Semashev
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/thread/thread.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/sync/locks/unique_lock.hpp>
#include <boost/sync/mutexes/mutex.hpp>
#include <boost/sync/condition_variables/condition_variable.hpp>
#include <boost/sync/condition_variables/condition_variable_any.hpp>
#include <boost/sync/support/boost_date_time.hpp>
#include <boost/test/unit_test.hpp>
#include "utils.hpp"

bool fake_predicate()
{
    return false;
}

unsigned const timeout_seconds=2;
unsigned const timeout_grace=1;
boost::posix_time::milliseconds const timeout_resolution(100);

void do_test_timed_wait_times_out()
{
    boost::sync::condition_variable cond;
    boost::sync::mutex m;

    boost::posix_time::seconds const delay(timeout_seconds);
    boost::sync::unique_lock<boost::sync::mutex> lock(m);
    boost::system_time const start = boost::get_system_time();
    boost::system_time const timeout = start+delay;

    while(cond.timed_wait(lock,timeout)) {}

    boost::system_time const end = boost::get_system_time();
    BOOST_CHECK((delay-timeout_resolution)<=(end-start));
}

BOOST_AUTO_TEST_CASE(test_timed_wait_times_out)
{
    timed_test(&do_test_timed_wait_times_out, timeout_seconds+timeout_grace, execution_monitor::use_mutex);
}

void do_test_timed_wait_with_predicate_times_out()
{
    boost::sync::condition_variable cond;
    boost::sync::mutex m;

    boost::posix_time::seconds const delay(timeout_seconds);
    boost::sync::unique_lock<boost::sync::mutex> lock(m);
    boost::system_time const start = boost::get_system_time();
    boost::system_time const timeout = start+delay;

    bool const res=cond.timed_wait(lock,timeout,fake_predicate);

    boost::system_time const end=boost::get_system_time();
    BOOST_CHECK(!res);
    BOOST_CHECK((delay-timeout_resolution)<=(end-start));
}

BOOST_AUTO_TEST_CASE(test_timed_wait_with_predicate_times_out)
{
    timed_test(&do_test_timed_wait_with_predicate_times_out, timeout_seconds+timeout_grace, execution_monitor::use_mutex);
}

void do_test_relative_timed_wait_with_predicate_times_out()
{
    boost::sync::condition_variable cond;
    boost::sync::mutex m;

    boost::posix_time::seconds const delay(timeout_seconds);
    boost::sync::unique_lock<boost::sync::mutex> lock(m);
    boost::system_time const start=boost::get_system_time();

    bool const res=cond.timed_wait(lock,delay,fake_predicate);

    boost::system_time const end=boost::get_system_time();
    BOOST_CHECK(!res);
    BOOST_CHECK((delay-timeout_resolution)<=(end-start));
}

BOOST_AUTO_TEST_CASE(test_relative_timed_wait_with_predicate_times_out)
{
    timed_test(&do_test_relative_timed_wait_with_predicate_times_out, timeout_seconds+timeout_grace, execution_monitor::use_mutex);
}

void do_test_timed_wait_relative_times_out()
{
    boost::sync::condition_variable cond;
    boost::sync::mutex m;

    boost::posix_time::seconds const delay(timeout_seconds);
    boost::sync::unique_lock<boost::sync::mutex> lock(m);
    boost::system_time const start=boost::get_system_time();

    while(cond.timed_wait(lock,delay)) {}

    boost::system_time const end=boost::get_system_time();
    BOOST_CHECK((delay-timeout_resolution)<=(end-start));
}

BOOST_AUTO_TEST_CASE(test_timed_wait_relative_times_out)
{
    timed_test(&do_test_timed_wait_relative_times_out, timeout_seconds+timeout_grace, execution_monitor::use_mutex);
}

void do_test_cv_any_timed_wait_times_out()
{
    boost::sync::condition_variable_any cond;
    boost::sync::mutex m;

    boost::posix_time::seconds const delay(timeout_seconds);
    boost::sync::unique_lock<boost::sync::mutex> lock(m);
    boost::system_time const start=boost::get_system_time();
    boost::system_time const timeout=start+delay;

    while(cond.timed_wait(lock,timeout)) {}

    boost::system_time const end=boost::get_system_time();
    BOOST_CHECK((delay-timeout_resolution)<=(end-start));
}

BOOST_AUTO_TEST_CASE(test_cv_any_timed_wait_times_out)
{
    timed_test(&do_test_cv_any_timed_wait_times_out, timeout_seconds+timeout_grace, execution_monitor::use_mutex);
}

void do_test_cv_any_timed_wait_with_predicate_times_out()
{
    boost::sync::condition_variable_any cond;
    boost::sync::mutex m;

    boost::posix_time::seconds const delay(timeout_seconds);
    boost::sync::unique_lock<boost::sync::mutex> lock(m);
    boost::system_time const start=boost::get_system_time();
    boost::system_time const timeout=start+delay;

    bool const res=cond.timed_wait(lock,timeout,fake_predicate);

    boost::system_time const end=boost::get_system_time();
    BOOST_CHECK(!res);
    BOOST_CHECK((delay-timeout_resolution)<=(end-start));
}

BOOST_AUTO_TEST_CASE(test_cv_any_timed_wait_with_predicate_times_out)
{
    timed_test(&do_test_cv_any_timed_wait_with_predicate_times_out, timeout_seconds+timeout_grace, execution_monitor::use_mutex);
}

void do_test_cv_any_relative_timed_wait_with_predicate_times_out()
{
    boost::sync::condition_variable_any cond;
    boost::sync::mutex m;

    boost::posix_time::seconds const delay(timeout_seconds);
    boost::sync::unique_lock<boost::sync::mutex> lock(m);
    boost::system_time const start=boost::get_system_time();

    bool const res=cond.timed_wait(lock,delay,fake_predicate);

    boost::system_time const end=boost::get_system_time();
    BOOST_CHECK(!res);
    BOOST_CHECK((delay-timeout_resolution)<=(end-start));
}

BOOST_AUTO_TEST_CASE(test_cv_any_relative_timed_wait_with_predicate_times_out)
{
    timed_test(&do_test_cv_any_relative_timed_wait_with_predicate_times_out, timeout_seconds+timeout_grace, execution_monitor::use_mutex);
}

void do_test_cv_any_timed_wait_relative_times_out()
{
    boost::sync::condition_variable_any cond;
    boost::sync::mutex m;

    boost::posix_time::seconds const delay(timeout_seconds);
    boost::sync::unique_lock<boost::sync::mutex> lock(m);
    boost::system_time const start=boost::get_system_time();

    while(cond.timed_wait(lock,delay)) {}

    boost::system_time const end=boost::get_system_time();
    BOOST_CHECK((delay-timeout_resolution)<=(end-start));
}

BOOST_AUTO_TEST_CASE(test_cv_any_timed_wait_relative_times_out)
{
    timed_test(&do_test_cv_any_timed_wait_relative_times_out, timeout_seconds+timeout_grace, execution_monitor::use_mutex);
}
