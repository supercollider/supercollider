/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2007-2008 Anthony Williams
 * (C) Copyright 2012-2013 Vicente J. Botet Escriba
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   detail/mutexes/mutex_windows.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_MUTEXES_MUTEX_WINDOWS_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_MUTEXES_MUTEX_WINDOWS_HPP_INCLUDED_

#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/mutexes/basic_mutex_windows.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE {

class mutex
{
public:
    typedef void _is_condition_variable_compatible;

private:
    sync::detail::windows::basic_mutex m_mutex;

public:
#if !defined(BOOST_NO_CXX11_CONSTEXPR)
#define BOOST_SYNC_DEFINES_MUTEX_CONSTEXPR_CONSTRUCTOR
#endif

    BOOST_CONSTEXPR mutex() BOOST_NOEXCEPT : m_mutex() {}

    void lock()
    {
        m_mutex.lock();
    }

    void unlock() BOOST_NOEXCEPT
    {
        m_mutex.unlock();
    }

    bool try_lock()
    {
        return m_mutex.try_lock();
    }

    BOOST_DELETED_FUNCTION(mutex(mutex const&))
    BOOST_DELETED_FUNCTION(mutex& operator= (mutex const&))
};

} // namespace winnt

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_MUTEXES_MUTEX_WINDOWS_HPP_INCLUDED_
