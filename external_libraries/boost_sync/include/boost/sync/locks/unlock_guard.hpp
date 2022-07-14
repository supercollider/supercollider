/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   locks/unlock_guard.hpp
 *
 * \brief  This header defines a scope guard temporarily releases a lock.
 */

#ifndef BOOST_SYNC_LOCKS_UNLOCK_GUARD_HPP_INCLUDED_
#define BOOST_SYNC_LOCKS_UNLOCK_GUARD_HPP_INCLUDED_

#include <boost/sync/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <boost/sync/locks/unlock_guard_fwd.hpp>

#include <boost/sync/detail/header.hpp>

namespace boost {

namespace sync {

/*!
 * \brief A scope guard that releases a lock
 */
template< typename Lockable >
class unlock_guard
{
public:
    // Note: naming is for compatibility with other lock types
    typedef Lockable mutex_type;

private:
    mutex_type& m_lockable;

public:
    explicit unlock_guard(mutex_type& m) BOOST_NOEXCEPT : m_lockable(m)
    {
        m.unlock();
    }

    ~unlock_guard() BOOST_NOEXCEPT_IF(false)
    {
        m_lockable.lock();
    }

    BOOST_DELETED_FUNCTION(unlock_guard(unlock_guard const&))
    BOOST_DELETED_FUNCTION(unlock_guard& operator= (unlock_guard const&))
};

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_LOCKS_UNLOCK_GUARD_HPP_INCLUDED_
