/*
 *             Copyright Andrey Semashev 2013.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   chrono_system_clock_time_t_mismatch.cpp
 *
 * \brief  This test verifies that \c chrono::system_clock has the same epoch time point as \c std::time_t
 */

#include <boost/test/unit_test.hpp>

#include <ctime>
#include <boost/chrono/system_clocks.hpp>
#include <boost/sync/detail/config.hpp>

#if !defined(BOOST_NO_CXX11_HDR_CHRONO)
#define BOOST_SYNC_DETAIL_TEST_STD_CHRONO
#endif

#if !defined(BOOST_SYNC_DETAIL_STD_CHRONO_TIME_T_MISMATCH)
#define BOOST_SYNC_DETAIL_CHECK_CLOCK(x) BOOST_CHECK_EQUAL(x, (std::time_t)0)
#else
#define BOOST_SYNC_DETAIL_CHECK_CLOCK(x) BOOST_CHECK_NE(x, (std::time_t)0)
#endif

#if defined(BOOST_SYNC_DETAIL_TEST_STD_CHRONO)
#include <chrono>

BOOST_AUTO_TEST_CASE(std_chrono_system_clock_time_t_mismatch)
{
    BOOST_SYNC_DETAIL_CHECK_CLOCK(std::chrono::system_clock::to_time_t(std::chrono::system_clock::time_point()));
}

BOOST_AUTO_TEST_CASE(std_chrono_system_clock_time_t_now_mismatch)
{
    std::time_t t1, t3;
    std::chrono::system_clock::time_point chrono_t2;
    do
    {
        t1 = std::time(0);
        chrono_t2 = std::chrono::system_clock::now();
        t3 = std::time(0);
    }
    while (t1 != t3);
    std::time_t t2 = std::chrono::system_clock::to_time_t(chrono_t2);

    // to_time_t is allowed to perform arithmetic rounding to seconds
    if (t2 > t1)
    {
        BOOST_CHECK_EQUAL((t2 - t1), 1);
    }
    else
    {
        BOOST_CHECK_EQUAL(t1, t2);
    }
}
#endif

BOOST_AUTO_TEST_CASE(boost_chrono_system_clock_time_t_mismatch)
{
    BOOST_CHECK_EQUAL(boost::chrono::system_clock::to_time_t(boost::chrono::system_clock::time_point()), (std::time_t)0);
}

BOOST_AUTO_TEST_CASE(boost_chrono_system_clock_time_t_now_mismatch)
{
    std::time_t t1, t3;
    boost::chrono::system_clock::time_point chrono_t2;
    do
    {
        t1 = std::time(0);
        chrono_t2 = boost::chrono::system_clock::now();
        t3 = std::time(0);
    }
    while (t1 != t3);
    std::time_t t2 = boost::chrono::system_clock::to_time_t(chrono_t2);

    // to_time_t is allowed to perform arithmetic rounding to seconds
    if (t2 > t1)
    {
        BOOST_CHECK_EQUAL((t2 - t1), 1);
    }
    else
    {
        BOOST_CHECK_EQUAL(t1, t2);
    }
}
