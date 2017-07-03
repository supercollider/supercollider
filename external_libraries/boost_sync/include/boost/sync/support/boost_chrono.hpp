/*
 *          Copyright Andrey Semashev 2013.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   support/boost_chrono.hpp
 *
 * \brief  This header enables support for Boost.Chrono time units.
 */

#ifndef BOOST_SYNC_SUPPORT_BOOST_CHRONO_HPP_INCLUDED_
#define BOOST_SYNC_SUPPORT_BOOST_CHRONO_HPP_INCLUDED_

#include <boost/ratio/ratio.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/chrono/time_point.hpp>
#include <boost/chrono/system_clocks.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/time_units.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

template< typename Rep, typename Period >
struct time_traits< boost::chrono::duration< Rep, Period > >
{
    typedef time_duration_tag tag;
    typedef system_duration unit_type;

    static BOOST_CONSTEXPR_OR_CONST bool is_specialized = true;

    static system_duration to_sync_unit(boost::chrono::duration< Rep, Period > const& dur) BOOST_NOEXCEPT
    {
        typedef boost::chrono::duration< system_duration::native_type, boost::ratio< 1, system_duration::subsecond_fraction > > system_chrono_duration;
        return system_duration(boost::chrono::duration_cast< system_chrono_duration >(dur).count());
    }
};

template< typename Clock, typename Duration >
struct time_traits< boost::chrono::time_point< Clock, Duration > >
{
    typedef time_point_tag tag;
    typedef chrono_time_point< boost::chrono::time_point< Clock, Duration > > unit_type;

    static BOOST_CONSTEXPR_OR_CONST bool is_specialized = true;

    static unit_type to_sync_unit(boost::chrono::time_point< Clock, Duration > const& point) BOOST_NOEXCEPT
    {
        return unit_type(point);
    }
};

template< typename Duration >
struct time_traits< boost::chrono::time_point< boost::chrono::system_clock, Duration > >
{
    typedef time_point_tag tag;
    typedef system_time_point unit_type;

    static BOOST_CONSTEXPR_OR_CONST bool is_specialized = true;

    static system_time_point to_sync_unit(boost::chrono::time_point< boost::chrono::system_clock, Duration > const& point) BOOST_NOEXCEPT
    {
        typedef typename boost::chrono::time_point< boost::chrono::system_clock, Duration >::duration duration;
        // system_clock in Boost.Chrono always have epoch at 1st January 1970
        return system_time_point(time_traits< duration >::to_sync_unit(point.time_since_epoch()));
    }
};

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_SUPPORT_BOOST_CHRONO_HPP_INCLUDED_
