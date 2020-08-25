/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2007 Anthony Williams
 * (C) Copyright 2011-2012 Vicente J. Botet Escriba
 * (C) Copyright 2013 Andrey Semashev
 */

#ifndef BOOST_SYNC_LOCKS_LOCK_OPTIONS_HPP_INCLUDED_
#define BOOST_SYNC_LOCKS_LOCK_OPTIONS_HPP_INCLUDED_

#include <boost/sync/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <boost/sync/detail/header.hpp>

namespace boost {

namespace sync {

struct defer_lock_t
{
};
struct try_to_lock_t
{
};
struct adopt_lock_t
{
};

BOOST_CONSTEXPR_OR_CONST defer_lock_t defer_lock = {};
BOOST_CONSTEXPR_OR_CONST try_to_lock_t try_to_lock = {};
BOOST_CONSTEXPR_OR_CONST adopt_lock_t adopt_lock = {};

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_LOCKS_LOCK_OPTIONS_HPP_INCLUDED_
