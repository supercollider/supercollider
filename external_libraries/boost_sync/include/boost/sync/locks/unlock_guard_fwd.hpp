/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   locks/unlock_guard_fwd.hpp
 *
 * \brief  This header declares a scope guard that temporarily releases the lock.
 */

#ifndef BOOST_SYNC_LOCKS_UNLOCK_GUARD_FWD_HPP_INCLUDED_
#define BOOST_SYNC_LOCKS_UNLOCK_GUARD_FWD_HPP_INCLUDED_

#include <boost/sync/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <boost/sync/detail/header.hpp>

namespace boost {

namespace sync {

/*!
 * \brief An unlock scope guard
 */
template< typename Lockable >
class unlock_guard;

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_LOCKS_UNLOCK_GUARD_FWD_HPP_INCLUDED_
