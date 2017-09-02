/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2012-2013 Vicente J. Botet Escriba
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   lockable_wrapper.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_LOCKABLE_WRAPPER_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_LOCKABLE_WRAPPER_HPP_INCLUDED_

#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

template< typename Mutex, typename Option = void >
struct lockable_wrapper
{
    typedef Mutex mutex_type;
    typedef Option lock_option;

    mutex_type* m_mutex;

    explicit lockable_wrapper(mutex_type& m) : m_mutex(&m)
    {
    }
};

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_LOCKABLE_WRAPPER_HPP_INCLUDED_
