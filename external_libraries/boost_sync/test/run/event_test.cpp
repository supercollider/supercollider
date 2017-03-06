// Copyright (C) 2013 Tim Blechmann
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>

#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include <boost/sync/events.hpp>
#include <boost/sync/support/boost_chrono.hpp>

#include <boost/typeof/typeof.hpp>

BOOST_AUTO_TEST_CASE(test_event_post_wait)
{
    boost::sync::manual_reset_event ev;

    ev.set();
    ev.wait();

    BOOST_REQUIRE( ev.try_wait() == true );

    ev.reset();

    BOOST_REQUIRE( ev.try_wait() == false );
}


BOOST_AUTO_TEST_CASE(test_event_post_try_wait)
{
    boost::sync::manual_reset_event ev;

    BOOST_REQUIRE( ev.try_wait() == false );

    ev.set();

    BOOST_REQUIRE( ev.try_wait() == true );
}

BOOST_AUTO_TEST_CASE(test_event_post_wait_autoreset)
{
    boost::sync::auto_reset_event ev;

    ev.post();
    BOOST_REQUIRE( ev.try_wait() == true );
    BOOST_REQUIRE( ev.try_wait() == false );
}


BOOST_AUTO_TEST_CASE(test_event_reset)
{
    boost::sync::manual_reset_event ev;

    BOOST_REQUIRE( ev.try_wait() == false );
    ev.set();
    BOOST_REQUIRE( ev.try_wait() == true );
    ev.reset();
    BOOST_REQUIRE( ev.try_wait() == false );
}

inline void post_event(boost::sync::manual_reset_event& evt)
{
    evt.set();
}

inline void post_event(boost::sync::auto_reset_event& evt)
{
    evt.post();
}

template <typename EventType>
struct event_wait_and_post_test
{
    void run()
    {
        boost::thread post_thread(boost::bind(&event_wait_and_post_test::wait_and_post, this));
        ev_.wait();
    }

    void wait_and_post()
    {
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
        (post_event)(ev_);
    }

    EventType ev_;
    boost::thread thread_;
};

BOOST_AUTO_TEST_CASE(event_wait_and_post)
{
    event_wait_and_post_test<boost::sync::manual_reset_event> test;
    test.run();
}

BOOST_AUTO_TEST_CASE(event_wait_and_post_autoreset)
{
    event_wait_and_post_test<boost::sync::auto_reset_event> test;
    test.run();
}

BOOST_AUTO_TEST_CASE(test_event_wait_for)
{
    using namespace boost;

    boost::sync::manual_reset_event ev;

    BOOST_AUTO(start, chrono::system_clock::now());

    BOOST_REQUIRE( ev.try_wait() == false );

    BOOST_REQUIRE(!ev.wait_for(chrono::milliseconds(500)));

    BOOST_REQUIRE( ev.try_wait() == false );


    BOOST_AUTO(end, chrono::system_clock::now());
    BOOST_AUTO(wait_time, end - start);

    // guessing!
    BOOST_REQUIRE( wait_time > chrono::milliseconds(450) );
    BOOST_REQUIRE( wait_time < chrono::milliseconds(1000) );

    ev.set();

    BOOST_REQUIRE(ev.wait_for(chrono::milliseconds(500)));
}

BOOST_AUTO_TEST_CASE(test_event_wait_until)
{
    using namespace boost;

    boost::sync::manual_reset_event ev;
    {
        BOOST_AUTO(now, chrono::system_clock::now());
        BOOST_AUTO(timeout, now + chrono::milliseconds(500));

        BOOST_REQUIRE(!ev.wait_until(timeout));

        BOOST_AUTO(end, chrono::system_clock::now());
        BOOST_AUTO(timeout_delta, end - timeout);

        // guessing!
        BOOST_REQUIRE( timeout_delta > chrono::milliseconds(-400) );
        BOOST_REQUIRE( timeout_delta < chrono::milliseconds(400) );
    }

    ev.set();

    {
        BOOST_AUTO(start,   chrono::system_clock::now());
        BOOST_AUTO(timeout, start + chrono::milliseconds(500));

        BOOST_REQUIRE(ev.wait_until(timeout));

        BOOST_AUTO(end, chrono::system_clock::now());

        // guessing!
        BOOST_REQUIRE( (end - start) < chrono::milliseconds(100) );
    }
}
