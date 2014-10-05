/*
 *             Copyright Andrey Semashev 2013.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   time_units.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_TIME_UNITS_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_TIME_UNITS_HPP_INCLUDED_

#include <time.h>
#include <boost/cstdint.hpp>
#include <boost/sync/detail/config.hpp>

#if defined(BOOST_SYNC_DETAIL_PLATFORM_WINAPI)
#include <boost/detail/winapi/time.hpp>
#elif defined(BOOST_SYNC_DETAIL_PLATFORM_MACH)
#include <sys/time.h> // gettimeofday, timeval
#endif

#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

class system_duration
{
public:
    typedef int64_t native_type;

#if defined(BOOST_SYNC_DETAIL_PLATFORM_WINAPI)
    // The native duration is in milliseconds
    static BOOST_CONSTEXPR_OR_CONST uint64_t subsecond_fraction = 1000u;
#else
    // The native duration is in nanoseconds
    static BOOST_CONSTEXPR_OR_CONST uint64_t subsecond_fraction = 1000000000u;
#endif

private:
    native_type m_value;

public:
    BOOST_CONSTEXPR system_duration() BOOST_NOEXCEPT : m_value(0) {}
    explicit system_duration(native_type value) BOOST_NOEXCEPT : m_value(value) {}

    native_type get() const BOOST_NOEXCEPT { return m_value; }

    system_duration& operator+= (system_duration const& that) BOOST_NOEXCEPT
    {
        m_value += that.m_value;
        return *this;
    }
    system_duration& operator-= (system_duration const& that) BOOST_NOEXCEPT
    {
        m_value -= that.m_value;
        return *this;
    }
    system_duration operator- () const BOOST_NOEXCEPT
    {
        return system_duration(-m_value);
    }

    friend system_duration operator+ (system_duration left, system_duration const& right) BOOST_NOEXCEPT
    {
        left += right;
        return left;
    }
    friend system_duration operator- (system_duration left, system_duration const& right) BOOST_NOEXCEPT
    {
        left -= right;
        return left;
    }
};

#if defined(BOOST_SYNC_DETAIL_PLATFORM_WINAPI)

class system_time_point
{
public:
    typedef uint64_t native_type;
    // The native subsecond precision is 100 nanoseconds
    static BOOST_CONSTEXPR_OR_CONST uint64_t subsecond_fraction = 10000000u;

private:
    //! 100 nanosecond units since 1601-Jan-01 (i.e. equivalent to FILETIME)
    uint64_t m_value;

public:
    BOOST_CONSTEXPR system_time_point() BOOST_NOEXCEPT : m_value(0) {}
    explicit system_time_point(time_t t, unsigned int subsecond = 0) BOOST_NOEXCEPT :
        m_value(static_cast< uint64_t >(t) * subsecond_fraction + subsecond + 116444736000000000ull)
    {
    }

    //! Creates time point from duration since 1970-Jan-01
    explicit system_time_point(system_duration dur) BOOST_NOEXCEPT :
        m_value(dur.get() * 10000u + 116444736000000000ull)
    {
    }

    native_type const& get() const BOOST_NOEXCEPT { return m_value; }

    static system_time_point now() BOOST_NOEXCEPT
    {
        system_time_point res;
        boost::detail::winapi::GetSystemTimeAsFileTime(reinterpret_cast< boost::detail::winapi::FILETIME_* >(&res.m_value));
        return res;
    }

    system_time_point& operator+= (system_duration const& dur) BOOST_NOEXCEPT
    {
        m_value += dur.get() * 10000u;
        return *this;
    }
    system_time_point& operator-= (system_duration const& dur) BOOST_NOEXCEPT
    {
        m_value -= dur.get() * 10000u;
        return *this;
    }

    friend system_time_point operator+ (system_time_point left, system_duration const& right) BOOST_NOEXCEPT
    {
        left += right;
        return left;
    }
    friend system_time_point operator- (system_time_point left, system_duration const& right) BOOST_NOEXCEPT
    {
        left -= right;
        return left;
    }

    friend system_duration operator- (system_time_point const& left, system_time_point const& right) BOOST_NOEXCEPT
    {
        return system_duration(static_cast< system_duration::native_type >(left.m_value - right.m_value) / 10000u);
    }
};

#else

class system_time_point
{
public:
    typedef struct ::timespec native_type;
    // The native subsecond precision is nanoseconds
    static BOOST_CONSTEXPR_OR_CONST uint64_t subsecond_fraction = system_duration::subsecond_fraction;

private:
    //! Time, since 1970-Jan-01
    native_type m_value;

public:
    BOOST_CONSTEXPR system_time_point() BOOST_NOEXCEPT : m_value() {}
    explicit system_time_point(time_t t, unsigned int subsecond = 0) BOOST_NOEXCEPT
    {
        m_value.tv_sec = t;
        m_value.tv_nsec = subsecond;
    }

    //! Creates time point from duration since 1970-Jan-01
    explicit system_time_point(system_duration dur) BOOST_NOEXCEPT
    {
        m_value.tv_sec = dur.get() / subsecond_fraction;
        m_value.tv_nsec = dur.get() % subsecond_fraction;
    }

    native_type const& get() const BOOST_NOEXCEPT { return m_value; }

    static system_time_point now() BOOST_NOEXCEPT
    {
#if defined(BOOST_HAS_CLOCK_GETTIME)
        system_time_point t;
        ::clock_gettime(CLOCK_REALTIME, &t.m_value);
        return t;
#elif defined(BOOST_SYNC_DETAIL_PLATFORM_MACH)
        ::timeval tv;
        ::gettimeofday(&tv, 0);
        return system_time_point(tv.tv_sec, tv.tv_usec * (subsecond_fraction / 1000000u));
#else
        return system_time_point(::time(0));
#endif
    }

    system_time_point& operator+= (system_duration const& dur) BOOST_NOEXCEPT
    {
        int64_t nsec = static_cast< int64_t >(m_value.tv_nsec) + dur.get();
        int64_t tv_nsec = nsec % system_duration::subsecond_fraction;
        if (tv_nsec < 0)
        {
            tv_nsec += subsecond_fraction;
            --m_value.tv_sec;
        }
        m_value.tv_nsec = tv_nsec;
        m_value.tv_sec += nsec / system_duration::subsecond_fraction;

        return *this;
    }
    system_time_point& operator-= (system_duration const& dur) BOOST_NOEXCEPT
    {
        return operator+= (-dur);
    }

    friend system_time_point operator+ (system_time_point left, system_duration const& right) BOOST_NOEXCEPT
    {
        left += right;
        return left;
    }
    friend system_time_point operator- (system_time_point left, system_duration const& right) BOOST_NOEXCEPT
    {
        left -= right;
        return left;
    }

    friend system_duration operator- (system_time_point const& left, system_time_point const& right) BOOST_NOEXCEPT
    {
        int64_t seconds = static_cast< int64_t >(left.m_value.tv_sec) - static_cast< int64_t >(right.m_value.tv_sec);
        int64_t nseconds = static_cast< int64_t >(left.m_value.tv_nsec) - static_cast< int64_t >(right.m_value.tv_nsec);
        return system_duration(seconds * system_duration::subsecond_fraction + nseconds);
    }
};

#endif

template< typename TimePoint >
class chrono_time_point :
    public TimePoint
{
public:
    typedef TimePoint time_point;
    typedef typename time_point::clock clock;
    typedef typename time_point::duration duration;

private:
    time_point m_value;

public:
    BOOST_DEFAULTED_FUNCTION(chrono_time_point(), : m_value() {})
    BOOST_DEFAULTED_FUNCTION(chrono_time_point(chrono_time_point const& that), : m_value(that.m_value) {})

    explicit chrono_time_point(time_point const& that) : m_value(that)
    {
    }

    template< typename T >
    explicit chrono_time_point(T const& arg) : m_value(arg)
    {
    }

    time_point const& get() const { return m_value; }

    static chrono_time_point now() { return chrono_time_point(clock::now()); }

    chrono_time_point& operator+= (duration const& dur)
    {
        m_value += dur;
        return *this;
    }
    chrono_time_point& operator-= (duration const& dur)
    {
        m_value -= dur;
        return *this;
    }

    friend chrono_time_point operator+ (chrono_time_point left, duration const& right)
    {
        left.m_value += right;
        return left;
    }
    friend chrono_time_point operator- (chrono_time_point left, duration const& right)
    {
        left.m_value -= right;
        return left;
    }

    friend duration operator- (chrono_time_point const& left, chrono_time_point const& right)
    {
        return left.m_value - right.m_value;
    }
};

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_TIME_UNITS_HPP_INCLUDED_
