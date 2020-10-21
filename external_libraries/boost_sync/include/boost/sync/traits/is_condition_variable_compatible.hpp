/*
 *          Copyright Andrey Semashev 2013.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   traits/is_condition_variable_compatible.hpp
 *
 * \brief  This header defines \c is_condition_variable_compatible type trait.
 */

#ifndef BOOST_SYNC_TRAITS_IS_CONDITION_VARIABLE_COMPATIBLE_HPP_INCLUDED_
#define BOOST_SYNC_TRAITS_IS_CONDITION_VARIABLE_COMPATIBLE_HPP_INCLUDED_

#include <boost/mpl/bool.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

template< typename Mutex, typename Void = void >
struct is_condition_variable_compatible : mpl::false_ {};

template< typename Mutex >
struct is_condition_variable_compatible< Mutex, typename Mutex::_is_condition_variable_compatible > : mpl::true_ {};

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_TRAITS_IS_CONDITION_VARIABLE_COMPATIBLE_HPP_INCLUDED_
