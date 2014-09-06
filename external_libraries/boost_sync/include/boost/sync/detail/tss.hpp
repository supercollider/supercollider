/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   tss.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html. In this file
 *         internal thread-specific storage API is defined.
 */

#ifndef BOOST_SYNC_DETAIL_TSS_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_TSS_HPP_INCLUDED_

#include <boost/sync/detail/link_config.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

typedef unsigned int thread_specific_key;
typedef void (*at_thread_exit_callback)(void*);

/*!
 * \brief Adds a callback to be invoked when the current thread terminates
 *
 * The specified callback will be invoked with the context pointer only once. The callback is
 * responsible for freeing any resources associated with the context upon being called.
 *
 * \param callback Pointer to the function to be invoked.
 * \param context Pointer to the context. This pointer will be passed to the \a callback.
 */
BOOST_SYNC_API void add_thread_exit_callback(at_thread_exit_callback callback, void* context);

/*!
 * \brief Creates a thread-specific key
 *
 * \param callback The callback to be called when a thread that used the key terminates. The callback will also be called when the key is deleted.
 * \param cleanup_at_delete If \c true, The \a callback will be called for all non-NULL thread-specific values associated with this key at the point
 *        of \c delete_thread_specific_key call. The user's code should be prepared that the cleanup function can be called not only in the context of the
 *        thread that initialized the specific value.
 *
 * \returns The created key.
 */
BOOST_SYNC_API thread_specific_key new_thread_specific_key(at_thread_exit_callback callback, bool cleanup_at_delete);
/*!
 * \brief Destroys the thread-specific key and all associated values.
 *
 * \param key The key to delete. The key cannot be used after executing this function.
 */
BOOST_SYNC_API void delete_thread_specific_key(thread_specific_key key) BOOST_NOEXCEPT;

/*!
 * \brief Acquires the value for the key associated with the calling thread.
 *
 * \param key The key that represents the value
 *
 * \returns The thread-specific value. Upon the first call, when the thread has not yet set the value, the function returns \c NULL.
 */
BOOST_SYNC_API void* get_thread_specific(thread_specific_key key) BOOST_NOEXCEPT;
/*!
 * \brief Installs a value for the key and the current thread.
 *
 * \param key The key that represents the value
 * \param p The value to set
 */
BOOST_SYNC_API void set_thread_specific(thread_specific_key key, void* p);

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_TSS_HPP_INCLUDED_
