/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   thread_specific/at_thread_exit.hpp
 *
 * \brief  This header defines \c at_thread_exit function.
 */

#ifndef BOOST_SYNC_THREAD_SPECIFIC_AT_THREAD_EXIT_HPP_INCLUDED_
#define BOOST_SYNC_THREAD_SPECIFIC_AT_THREAD_EXIT_HPP_INCLUDED_

#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/tss.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

template< typename Function >
void at_thread_exit_trampoline(void* p)
{
    Function* const fun = static_cast< Function* >(p);
    (*fun)();
    delete fun;
}

} // namespace detail

/*!
 * Registers a function to be called at the current thread termination. The function must not accept any arguments, its
 * return value is ignored. The function must not throw exceptions. If multiple functions are registered by the thread,
 * these functions will be invoked in the order reverse of their registration.
 *
 * \param fun The function to be called at thread termination.
 */
template< typename Function >
inline void at_thread_exit(Function const& fun)
{
    Function* p = new Function(fun);
    try
    {
        sync::detail::add_thread_exit_callback(&sync::detail::at_thread_exit_trampoline< Function >, p);
    }
    catch (...)
    {
        delete p;
        throw;
    }
}

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_THREAD_SPECIFIC_AT_THREAD_EXIT_HPP_INCLUDED_
