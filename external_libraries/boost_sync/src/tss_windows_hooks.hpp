/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2004 Michael Glassford
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   tss_windows_hooks.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_IMPL_TSS_WINDOWS_HOOKS_HPP_INCLUDED_
#define BOOST_SYNC_IMPL_TSS_WINDOWS_HOOKS_HPP_INCLUDED_

#include <boost/sync/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

namespace windows {

/*!
 * Function to be called when the exe or dll that uses Boost.Sync first starts or is first loaded.
 * Should be called only before the first call to \c on_thread_enter. Called automatically by
 * Boost.Sync when a method for doing so has been discovered. May be omitted; may be called multiple times.
 */
void on_process_enter();

/*!
 * Function to be called when the exe or dll that uses Boost.Sync first starts or is first loaded.
 * Should be called only after the last call to \c on_thread_exit. Called automatically by Boost.Sync
 * when a method for doing so has been discovered. Must not be omitted; may be called multiple times.
 */
void on_process_exit();

/*!
 * Function to be called just after a thread starts in an exe or dll that uses Boost.Sync.
 * Must be called in the context of the thread that is starting. Called automatically by Boost.Sync
 * when a method for doing so has been discovered. May be omitted; may be called multiple times.
 */
void on_thread_enter();

/*!
 * Function to be called just be fore a thread ends in an exe or dll that uses Boost.Sync.
 * Must be called in the context of the thread that is ending. Called automatically by Boost.Sync
 * when a method for doing so has been discovered. Must not be omitted; may be called multiple times.
 */
void on_thread_exit();

/*!
 * Dummy function used both to detect whether tss cleanup cleanup has been implemented and to force
 * it to be linked into the Boost.Sync library.
 */
void tss_cleanup_implemented();

} // namespace windows

} // namespace detail

} // namespace sync

} // namespace boost

#endif // BOOST_SYNC_IMPL_TSS_WINDOWS_HOOKS_HPP_INCLUDED_
