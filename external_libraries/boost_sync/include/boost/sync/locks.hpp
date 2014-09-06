/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   locks.hpp
 *
 * \brief  This header includes all lock types.
 */

#ifndef BOOST_SYNC_LOCKS_HPP_INCLUDED_
#define BOOST_SYNC_LOCKS_HPP_INCLUDED_

#include <boost/sync/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <boost/sync/locks/lock_guard.hpp>
#include <boost/sync/locks/unlock_guard.hpp>
#include <boost/sync/locks/shared_lock_guard.hpp>
#include <boost/sync/locks/unique_lock.hpp>
#include <boost/sync/locks/shared_lock.hpp>
#include <boost/sync/locks/upgrade_lock.hpp>

#endif // BOOST_SYNC_LOCKS_HPP_INCLUDED_
