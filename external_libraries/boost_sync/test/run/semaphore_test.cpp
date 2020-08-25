// Copyright (C) 2013 Tim Blechmann
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/core/lightweight_test.hpp>

#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/sync/semaphore.hpp>
#include <boost/sync/support/boost_chrono.hpp>
#include <boost/sync/support/boost_date_time.hpp>

#include <boost/typeof/typeof.hpp>

void test_semaphore_post_wait()
{
    boost::sync::semaphore sem;

    sem.post();
    sem.wait();
}

void test_semaphore_try_wait()
{
    boost::sync::semaphore sem;

    BOOST_TEST(!sem.try_wait());
    sem.post();
    BOOST_TEST(sem.try_wait());
}

void test_semaphore_initial_count()
{
    boost::sync::semaphore sem(2);

    BOOST_TEST(sem.try_wait());
    BOOST_TEST(sem.try_wait());
    BOOST_TEST(!sem.try_wait());
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

void test_semaphore_wait_and_post()
{
    semaphore_wait_and_post_test test;
    test.run();
}

void test_semaphore_wait_for()
{
    using namespace boost;

    boost::sync::semaphore sem;

    BOOST_AUTO(start, chrono::system_clock::now());

    BOOST_TEST(!sem.wait_for(chrono::milliseconds(500)));

    BOOST_AUTO(end, chrono::system_clock::now());
    BOOST_AUTO(wait_time, end - start);

    // guessing!
    BOOST_TEST( wait_time > chrono::milliseconds(450) );
    BOOST_TEST( wait_time < chrono::milliseconds(1000) );

    sem.post();

    BOOST_TEST(sem.wait_for(chrono::milliseconds(500)));
}

void test_semaphore_wait_until()
{
    using namespace boost;

    boost::sync::semaphore sem;
    {
        BOOST_AUTO(now, chrono::system_clock::now());
        BOOST_AUTO(timeout, now + chrono::milliseconds(500));

        BOOST_TEST(!sem.wait_until(timeout));

        BOOST_AUTO(end, chrono::system_clock::now());
        BOOST_AUTO(timeout_delta, end - timeout);

        // guessing!
        BOOST_TEST( timeout_delta > chrono::milliseconds(-400) );
        BOOST_TEST( timeout_delta < chrono::milliseconds(400) );
    }

    sem.post();

    {
        BOOST_AUTO(start, chrono::system_clock::now());
        BOOST_AUTO(timeout, start + chrono::milliseconds(500));

        BOOST_TEST(sem.wait_until(timeout));

        BOOST_AUTO(end, chrono::system_clock::now());

        // guessing!
        BOOST_TEST( (end - start) < chrono::milliseconds(100) );
    }
}

void test_semaphore_timed_wait()
{
    using namespace boost;

    boost::sync::semaphore sem;

    // Relative timeouts
    {
        BOOST_AUTO(start, chrono::system_clock::now());

        BOOST_TEST(!sem.timed_wait(chrono::milliseconds(500)));

        BOOST_AUTO(end, chrono::system_clock::now());
        BOOST_AUTO(wait_time, end - start);

        // guessing!
        BOOST_TEST( wait_time > chrono::milliseconds(450) );
        BOOST_TEST( wait_time < chrono::milliseconds(1000) );

        sem.post();

        BOOST_TEST(sem.timed_wait(chrono::milliseconds(500)));
    }

    {
        BOOST_AUTO(start, get_system_time());

        BOOST_TEST(!sem.timed_wait(posix_time::milliseconds(500)));

        BOOST_AUTO(end, get_system_time());
        BOOST_AUTO(wait_time, end - start);

        // guessing!
        BOOST_TEST( wait_time > posix_time::milliseconds(450) );
        BOOST_TEST( wait_time < posix_time::milliseconds(1000) );

        sem.post();

        BOOST_TEST(sem.timed_wait(posix_time::milliseconds(500)));
    }

    // Absolute timeouts
    {
        BOOST_AUTO(now, chrono::system_clock::now());
        BOOST_AUTO(timeout, now + chrono::milliseconds(500));

        BOOST_TEST(!sem.timed_wait(timeout));

        BOOST_AUTO(end, chrono::system_clock::now());
        BOOST_AUTO(timeout_delta, end - timeout);

        // guessing!
        BOOST_TEST( timeout_delta > chrono::milliseconds(-400) );
        BOOST_TEST( timeout_delta < chrono::milliseconds(400) );
    }

    sem.post();

    {
        BOOST_AUTO(start, chrono::system_clock::now());
        BOOST_AUTO(timeout, start + chrono::milliseconds(500));

        BOOST_TEST(sem.timed_wait(timeout));

        BOOST_AUTO(end, chrono::system_clock::now());

        // guessing!
        BOOST_TEST( (end - start) < chrono::milliseconds(100) );
    }

    {
        BOOST_AUTO(now, get_system_time());
        BOOST_AUTO(timeout, now + posix_time::milliseconds(500));

        BOOST_TEST(!sem.timed_wait(timeout));

        BOOST_AUTO(end, get_system_time());
        BOOST_AUTO(timeout_delta, end - timeout);

        // guessing!
        BOOST_TEST( timeout_delta > posix_time::milliseconds(-400) );
        BOOST_TEST( timeout_delta < posix_time::milliseconds(400) );
    }

    sem.post();

    {
        BOOST_AUTO(start, get_system_time());
        BOOST_AUTO(timeout, start + posix_time::milliseconds(500));

        BOOST_TEST(sem.timed_wait(timeout));

        BOOST_AUTO(end, get_system_time());

        // guessing!
        BOOST_TEST( (end - start) < posix_time::milliseconds(100) );
    }

    {
        // timed wait after timeout
        boost::sync::semaphore sema;

        using namespace boost::chrono;

        BOOST_AUTO(start, steady_clock::now());
        BOOST_AUTO(timeout, start + milliseconds(100));

        boost::this_thread::sleep_for(milliseconds(500));

        sema.post();

        BOOST_TEST(sema.wait_until(timeout));
    }
}

int main()
{
    test_semaphore_post_wait();
    test_semaphore_try_wait();
    test_semaphore_initial_count();
    test_semaphore_wait_and_post();
    test_semaphore_wait_for();
    test_semaphore_wait_until();
    test_semaphore_timed_wait();

    return boost::report_errors();
}
