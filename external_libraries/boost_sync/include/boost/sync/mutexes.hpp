/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   mutexes.hpp
 *
 * \brief  This header includes all mutex types.
 */

#ifndef BOOST_SYNC_MUTEXES_HPP_INCLUDED_
#define BOOST_SYNC_MUTEXES_HPP_INCLUDED_

#include <boost/sync/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <boost/sync/mutexes/mutex.hpp>
#include <boost/sync/mutexes/shared_spin_mutex.hpp>
#include <boost/sync/mutexes/spin_mutex.hpp>
#include <boost/sync/mutexes/timed_mutex.hpp>

#endif // BOOST_SYNC_MUTEXES_HPP_INCLUDED_
