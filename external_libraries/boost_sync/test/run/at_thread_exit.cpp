/*
 *             Copyright Andrey Semashev 2013.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   at_thread_exit.cpp
 *
 * \brief  This test checks that \c at_thread_exit works
 */

#include <boost/test/unit_test.hpp>
#include <boost/thread/thread.hpp>
#include <boost/sync/thread_specific/at_thread_exit.hpp>

unsigned int called = 0;

struct my_at_thread_exit
{
    typedef void result_type;

    explicit my_at_thread_exit(unsigned int& n) : m_called(&n) {}

    void operator() () const
    {
        ++(*m_called);
    }

private:
    unsigned int* m_called;
};

void my_thread_routine()
{
    boost::sync::at_thread_exit(my_at_thread_exit(called));
}


BOOST_AUTO_TEST_CASE(test_at_thread_exit)
{
    boost::thread t(&my_thread_routine);
    t.join();
    BOOST_CHECK_EQUAL(called, 1);
}
