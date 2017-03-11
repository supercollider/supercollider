#ifndef BOOST_SYNC_TEST_CONDITION_TEST_COMMON_HPP
#define BOOST_SYNC_TEST_CONDITION_TEST_COMMON_HPP
// Copyright (C) 2007 Anthony Williams
// Copyright (C) 2013 Andrey Semashev
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/sync/locks/unique_lock.hpp>
#include <boost/sync/mutexes/mutex.hpp>
#include <boost/sync/condition_variables/condition_variable.hpp>
#include <boost/sync/support/boost_date_time.hpp>

unsigned const timeout_seconds = 5;

struct wait_for_flag
{
    struct check_flag
    {
        bool const& flag;

        explicit check_flag(bool const& flag_) : flag(flag_)
        {
        }

        check_flag(check_flag const& that) : flag(that.flag)
        {
        }

        bool operator()() const
        {
            return flag;
        }

        BOOST_DELETED_FUNCTION(check_flag& operator=(check_flag const&))
    };

    boost::sync::mutex mutex;
    boost::sync::condition_variable cond_var;
    bool flag;
    unsigned woken;

    wait_for_flag():
        flag(false),woken(0)
    {
    }

    void wait_without_predicate()
    {
        boost::sync::unique_lock<boost::sync::mutex> lock(mutex);
        while(!flag)
        {
            cond_var.wait(lock);
        }
        ++woken;
    }

    void wait_with_predicate()
    {
        boost::sync::unique_lock<boost::sync::mutex> lock(mutex);
        cond_var.wait(lock,check_flag(flag));
        if(flag)
        {
            ++woken;
        }
    }

    void timed_wait_without_predicate()
    {
        boost::system_time const timeout = boost::get_system_time() + boost::posix_time::seconds(timeout_seconds);

        boost::sync::unique_lock<boost::sync::mutex> lock(mutex);
        while(!flag)
        {
            if(!cond_var.timed_wait(lock,timeout))
            {
                return;
            }
        }
        ++woken;
    }

    void timed_wait_with_predicate()
    {
        boost::system_time const timeout = boost::get_system_time() + boost::posix_time::seconds(timeout_seconds);
        boost::sync::unique_lock<boost::sync::mutex> lock(mutex);
        if(cond_var.timed_wait(lock,timeout,check_flag(flag)) && flag)
        {
            ++woken;
        }
    }
    void relative_timed_wait_with_predicate()
    {
        boost::sync::unique_lock<boost::sync::mutex> lock(mutex);
        if(cond_var.timed_wait(lock,boost::posix_time::seconds(timeout_seconds),check_flag(flag)) && flag)
        {
            ++woken;
        }
    }

    BOOST_DELETED_FUNCTION(wait_for_flag(wait_for_flag const&))
    BOOST_DELETED_FUNCTION(wait_for_flag& operator=(wait_for_flag const&))
};

#endif
