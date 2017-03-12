/*
 *          Copyright Andrey Semashev 2013.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   support/boost_date_time.hpp
 *
 * \brief  This header enables support for Boost.DateTime time units.
 */

#ifndef BOOST_SYNC_SUPPORT_BOOST_DATE_TIME_HPP_INCLUDED_
#define BOOST_SYNC_SUPPORT_BOOST_DATE_TIME_HPP_INCLUDED_

#include <boost/assert.hpp>
#include <boost/cstdint.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
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

template< typename T >
struct time_traits< T, typename T::_is_boost_date_time_duration >
{
    typedef time_duration_tag tag;
    typedef system_duration unit_type;

    static BOOST_CONSTEXPR_OR_CONST bool is_specialized = true;

    static system_duration to_sync_unit(T const& dur)
    {
        BOOST_ASSERT(!dur.is_special());

        typedef typename T::traits_type traits_type;
        enum
        {
            conversion_ratio = static_cast< uint64_t >(traits_type::ticks_per_second) >= system_duration::subsecond_fraction ?
                traits_type::ticks_per_second / system_duration::subsecond_fraction :
                system_duration::subsecond_fraction / traits_type::ticks_per_second
        };
        return system_duration(static_cast< uint64_t >(traits_type::ticks_per_second) >= system_duration::subsecond_fraction ?
            dur.ticks() / conversion_ratio : dur.ticks() * conversion_ratio);
    }
};

template< typename T >
struct time_traits< T, typename T::_is_boost_date_time_time_point >
{
    typedef time_point_tag tag;
    typedef system_time_point unit_type;

    static BOOST_CONSTEXPR_OR_CONST bool is_specialized = true;

    static system_time_point to_sync_unit(T const& point)
    {
        BOOST_ASSERT(!point.is_special());

        typedef typename T::date_type date_type;
        typedef typename T::time_duration_type time_duration_type;
        time_duration_type dur = point - T(date_type(1970, 1, 1));
        return system_time_point(time_traits< time_duration_type >::to_sync_unit(dur));
    }
};

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_SUPPORT_BOOST_DATE_TIME_HPP_INCLUDED_
