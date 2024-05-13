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

#include <boost/core/lightweight_test.hpp>

#include <ctime>
#include <boost/chrono/system_clocks.hpp>
#include <boost/sync/detail/config.hpp>

#if !defined(BOOST_NO_CXX11_HDR_CHRONO)
#define BOOST_SYNC_DETAIL_TEST_STD_CHRONO
#endif

#if !defined(BOOST_SYNC_DETAIL_STD_CHRONO_TIME_T_MISMATCH)
#define BOOST_SYNC_DETAIL_CHECK_CLOCK(x) BOOST_TEST_EQ(x, (std::time_t)0)
#else
#define BOOST_SYNC_DETAIL_CHECK_CLOCK(x) BOOST_TEST_NE(x, (std::time_t)0)
#endif

#if defined(BOOST_SYNC_DETAIL_TEST_STD_CHRONO)
#include <chrono>

void test_std_chrono_system_clock_time_t_mismatch()
{
    BOOST_SYNC_DETAIL_CHECK_CLOCK(std::chrono::system_clock::to_time_t(std::chrono::system_clock::time_point()));
}

void test_std_chrono_system_clock_time_t_now_mismatch()
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
        BOOST_TEST_EQ((t2 - t1), 1);
    }
    // std::time may also be implemented on top of system clock and perform arithmetic rounding
    else if (t1 > t2)
    {
        BOOST_TEST_EQ((t1 - t2), 1);
    }
    else
    {
        BOOST_TEST_EQ(t1, t2);
    }
}
#endif

void test_boost_chrono_system_clock_time_t_mismatch()
{
    BOOST_TEST_EQ(boost::chrono::system_clock::to_time_t(boost::chrono::system_clock::time_point()), (std::time_t)0);
}

void test_boost_chrono_system_clock_time_t_now_mismatch()
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
        BOOST_TEST_EQ((t2 - t1), 1);
    }
    else
    {
        BOOST_TEST_EQ(t1, t2);
    }
}

int main()
{
#if defined(BOOST_SYNC_DETAIL_TEST_STD_CHRONO)
    test_std_chrono_system_clock_time_t_mismatch();
    test_std_chrono_system_clock_time_t_now_mismatch();
#endif

    test_boost_chrono_system_clock_time_t_mismatch();
    test_boost_chrono_system_clock_time_t_now_mismatch();

    return boost::report_errors();
}
