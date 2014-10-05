/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2007-2008 Anthony Williams
 * (C) Copyright 2011-2012 Vicente J. Botet Escriba
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   detail/condition_variables/condition_variable_any_windows.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_CONDITION_VARIABLES_CONDITION_VARIABLE_ANY_WINDOWS_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_CONDITION_VARIABLES_CONDITION_VARIABLE_ANY_WINDOWS_HPP_INCLUDED_

#include <boost/utility/enable_if.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/time_units.hpp>
#include <boost/sync/detail/condition_variables/basic_condition_variable_windows.hpp>
#include <boost/sync/condition_variables/cv_status.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE {

class condition_variable_any
{
private:
    sync::detail::windows::basic_condition_variable m_cond;

public:
#if !defined(BOOST_NO_CXX11_CONSTEXPR)
#define BOOST_SYNC_DEFINES_CONDITION_VARIABLE_ANY_CONSTEXPR_CONSTRUCTOR
#endif

    BOOST_CONSTEXPR condition_variable_any() BOOST_NOEXCEPT : m_cond()
    {
    }

    void notify_one() BOOST_NOEXCEPT
    {
        m_cond.notify_one();
    }

    void notify_all() BOOST_NOEXCEPT
    {
        m_cond.notify_all();
    }

    template< typename Lock >
    void wait(Lock& lock)
    {
        m_cond.wait(lock);
    }

    template< typename Lock, typename Predicate >
    void wait(Lock& lock, Predicate pred)
    {
        while (!pred())
            m_cond.wait(lock);
    }

    template< typename Lock, typename Time >
    typename enable_if_c< sync::detail::time_traits< Time >::is_specialized, bool >::type
    timed_wait(Lock& lock, Time const& t)
    {
        return m_cond.timed_wait(lock, sync::detail::time_traits< Time >::to_sync_unit(t)) == sync::cv_status::no_timeout;
    }

    template< typename Lock, typename TimePoint, typename Predicate >
    typename detail::enable_if_tag< TimePoint, detail::time_point_tag, bool >::type
    timed_wait(Lock& lock, TimePoint const& t, Predicate pred)
    {
        typedef typename sync::detail::time_traits< TimePoint >::unit_type unit_type;
        unit_type abs_timeout = sync::detail::time_traits< TimePoint >::to_sync_unit(t);
        while (!pred())
        {
            if (m_cond.timed_wait(lock, abs_timeout) != sync::cv_status::no_timeout)
                return pred();
        }
        return true;
    }

    template< typename Lock, typename Duration, typename Predicate >
    typename detail::enable_if_tag< Duration, detail::time_duration_tag, bool >::type
    timed_wait(Lock& lock, Duration const& t, Predicate pred)
    {
        sync::detail::system_time_point abs_timeout = sync::detail::system_time_point::now() + sync::detail::time_traits< Duration >::to_sync_unit(t);
        while (!pred())
        {
            if (m_cond.timed_wait(lock, abs_timeout) != sync::cv_status::no_timeout)
                return pred();
        }
        return true;
    }

    template< typename Lock, typename TimePoint >
    typename detail::enable_if_tag< TimePoint, detail::time_point_tag, sync::cv_status >::type
    wait_until(Lock& lock, TimePoint const& abs_time)
    {
        return m_cond.timed_wait(lock, sync::detail::time_traits< TimePoint >::to_sync_unit(abs_time));
    }

    template< typename Lock, typename TimePoint, typename Predicate >
    typename detail::enable_if_tag< TimePoint, detail::time_point_tag, bool >::type
    wait_until(Lock& lock, TimePoint const& abs_time, Predicate pred)
    {
        typedef typename sync::detail::time_traits< TimePoint >::unit_type unit_type;
        unit_type abs_timeout = sync::detail::time_traits< TimePoint >::to_sync_unit(abs_time);
        while (!pred())
        {
            if (m_cond.timed_wait(lock, abs_timeout) != sync::cv_status::no_timeout)
                return pred();
        }
        return true;
    }

    template< typename Lock, typename Duration >
    typename detail::enable_if_tag< Duration, detail::time_duration_tag, sync::cv_status >::type
    wait_for(Lock& lock, Duration const& rel_time)
    {
        return m_cond.timed_wait(lock, sync::detail::time_traits< Duration >::to_sync_unit(rel_time));
    }

    template< typename Lock, typename Duration, typename Predicate >
    typename detail::enable_if_tag< Duration, detail::time_duration_tag, bool >::type
    wait_for(Lock& lock, Duration const& rel_time, Predicate pred)
    {
        sync::detail::system_time_point abs_timeout = sync::detail::system_time_point::now() + sync::detail::time_traits< Duration >::to_sync_unit(rel_time);
        while (!pred())
        {
            if (m_cond.timed_wait(lock, abs_timeout) != sync::cv_status::no_timeout)
                return pred();
        }
        return true;
    }

    BOOST_DELETED_FUNCTION(condition_variable_any(condition_variable_any const&))
    BOOST_DELETED_FUNCTION(condition_variable_any& operator= (condition_variable_any const&))
};

} // namespace abi

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_CONDITION_VARIABLES_CONDITION_VARIABLE_ANY_WINDOWS_HPP_INCLUDED_
