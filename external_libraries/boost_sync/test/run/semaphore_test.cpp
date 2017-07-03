// Copyright (C) 2013 Tim Blechmann
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>

#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/sync/semaphore.hpp>
#include <boost/sync/support/boost_chrono.hpp>
#include <boost/sync/support/boost_date_time.hpp>

#include <boost/typeof/typeof.hpp>

BOOST_AUTO_TEST_CASE(test_semaphore_post_wait)
{
    boost::sync::semaphore sem;

    sem.post();
    sem.wait();
}

BOOST_AUTO_TEST_CASE(test_semaphore_try_wait)
{
    boost::sync::semaphore sem;

    BOOST_REQUIRE(!sem.try_wait());
    sem.post();
    BOOST_REQUIRE(sem.try_wait());
}

BOOST_AUTO_TEST_CASE(test_semaphore_initial_count)
{
    boost::sync::semaphore sem(2);

    BOOST_REQUIRE(sem.try_wait());
    BOOST_REQUIRE(sem.try_wait());
    BOOST_REQUIRE(!sem.try_wait());
}


struct semaphore_wait_and_post_test
{
    void run()
    {
        boost::thread post_thread(boost::bind(&semaphore_wait_and_post_test::wait_and_post, this));
        sem_.wait();
    }

    void wait_and_post()
    {
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
        sem_.post();
    }

    boost::sync::semaphore sem_;
    boost::thread thread_;
};

BOOST_AUTO_TEST_CASE(semaphore_wait_and_post)
{
    semaphore_wait_and_post_test test;
    test.run();
}

BOOST_AUTO_TEST_CASE(test_semaphore_wait_for)
{
    using namespace boost;

    boost::sync::semaphore sem;

    BOOST_AUTO(start, chrono::system_clock::now());

    BOOST_REQUIRE(!sem.wait_for(chrono::milliseconds(500)));

    BOOST_AUTO(end, chrono::system_clock::now());
    BOOST_AUTO(wait_time, end - start);

    // guessing!
    BOOST_REQUIRE( wait_time > chrono::milliseconds(450) );
    BOOST_REQUIRE( wait_time < chrono::milliseconds(1000) );

    sem.post();

    BOOST_REQUIRE(sem.wait_for(chrono::milliseconds(500)));
}

BOOST_AUTO_TEST_CASE(test_semaphore_wait_until)
{
    using namespace boost;

    boost::sync::semaphore sem;
    {
        BOOST_AUTO(now, chrono::system_clock::now());
        BOOST_AUTO(timeout, now + chrono::milliseconds(500));

        BOOST_REQUIRE(!sem.wait_until(timeout));

        BOOST_AUTO(end, chrono::system_clock::now());
        BOOST_AUTO(timeout_delta, end - timeout);

        // guessing!
        BOOST_REQUIRE( timeout_delta > chrono::milliseconds(-400) );
        BOOST_REQUIRE( timeout_delta < chrono::milliseconds(400) );
    }

    sem.post();

    {
        BOOST_AUTO(start, chrono::system_clock::now());
        BOOST_AUTO(timeout, start + chrono::milliseconds(500));

        BOOST_REQUIRE(sem.wait_until(timeout));

        BOOST_AUTO(end, chrono::system_clock::now());

        // guessing!
        BOOST_REQUIRE( (end - start) < chrono::milliseconds(100) );
    }
}

BOOST_AUTO_TEST_CASE(test_semaphore_timed_wait)
{
    using namespace boost;

    boost::sync::semaphore sem;

    // Relative timeouts
    {
        BOOST_AUTO(start, chrono::system_clock::now());

        BOOST_REQUIRE(!sem.timed_wait(chrono::milliseconds(500)));

        BOOST_AUTO(end, chrono::system_clock::now());
        BOOST_AUTO(wait_time, end - start);

        // guessing!
        BOOST_REQUIRE( wait_time > chrono::milliseconds(450) );
        BOOST_REQUIRE( wait_time < chrono::milliseconds(1000) );

        sem.post();

        BOOST_REQUIRE(sem.timed_wait(chrono::milliseconds(500)));
    }

    {
        BOOST_AUTO(start, get_system_time());

        BOOST_REQUIRE(!sem.timed_wait(posix_time::milliseconds(500)));

        BOOST_AUTO(end, get_system_time());
        BOOST_AUTO(wait_time, end - start);

        // guessing!
        BOOST_REQUIRE( wait_time > posix_time::milliseconds(450) );
        BOOST_REQUIRE( wait_time < posix_time::milliseconds(1000) );

        sem.post();

        BOOST_REQUIRE(sem.timed_wait(posix_time::milliseconds(500)));
    }

    // Absolute timeouts
    {
        BOOST_AUTO(now, chrono::system_clock::now());
        BOOST_AUTO(timeout, now + chrono::milliseconds(500));

        BOOST_REQUIRE(!sem.timed_wait(timeout));

        BOOST_AUTO(end, chrono::system_clock::now());
        BOOST_AUTO(timeout_delta, end - timeout);

        // guessing!
        BOOST_REQUIRE( timeout_delta > chrono::milliseconds(-400) );
        BOOST_REQUIRE( timeout_delta < chrono::milliseconds(400) );
    }

    sem.post();

    {
        BOOST_AUTO(start, chrono::system_clock::now());
        BOOST_AUTO(timeout, start + chrono::milliseconds(500));

        BOOST_REQUIRE(sem.timed_wait(timeout));

        BOOST_AUTO(end, chrono::system_clock::now());

        // guessing!
        BOOST_REQUIRE( (end - start) < chrono::milliseconds(100) );
    }

    {
        BOOST_AUTO(now, get_system_time());
        BOOST_AUTO(timeout, now + posix_time::milliseconds(500));

        BOOST_REQUIRE(!sem.timed_wait(timeout));

        BOOST_AUTO(end, get_system_time());
        BOOST_AUTO(timeout_delta, end - timeout);

        // guessing!
        BOOST_REQUIRE( timeout_delta > posix_time::milliseconds(-400) );
        BOOST_REQUIRE( timeout_delta < posix_time::milliseconds(400) );
    }

    sem.post();

    {
        BOOST_AUTO(start, get_system_time());
        BOOST_AUTO(timeout, start + posix_time::milliseconds(500));

        BOOST_REQUIRE(sem.timed_wait(timeout));

        BOOST_AUTO(end, get_system_time());

        // guessing!
        BOOST_REQUIRE( (end - start) < posix_time::milliseconds(100) );
    }
}
