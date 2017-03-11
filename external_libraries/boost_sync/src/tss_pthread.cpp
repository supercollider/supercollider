/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2004 Michael Glassford
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   tss_pthread.cpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#include <boost/sync/detail/config.hpp>

#if !defined(BOOST_SYNC_DETAIL_PLATFORM_WINAPI)

#include <cstddef>
#include <cstdlib>
#include <pthread.h>
#include <boost/sync/detail/tss.hpp>
#include <boost/sync/condition_variables/notify_all_at_thread_exit.hpp>
#include "tss_manager.hpp"
#include <boost/sync/detail/header.hpp>

namespace boost {

namespace sync {

namespace detail {

namespace {

static pthread_once_t init_tss_once_flag = PTHREAD_ONCE_INIT;
static tss_manager* tss_mgr = NULL;
#if defined(BOOST_SYNC_DETAIL_TLS)
static BOOST_SYNC_DETAIL_TLS tss_manager::thread_context* tss_context = NULL;
#else
static pthread_key_t tss_key;
#endif

extern "C" {

static void tss_cleanup(void* p)
{
    if (tss_manager::thread_context* ctx = static_cast< tss_manager::thread_context* >(p))
    {
        tss_mgr->destroy_thread_context(ctx);
    }
}

static void init_tss()
{
    try
    {
        tss_mgr = new tss_manager();
    }
    catch (...)
    {
        std::abort();
    }

#if !defined(BOOST_SYNC_DETAIL_TLS)
    if (pthread_key_create(&tss_key, &tss_cleanup) != 0)
        std::abort();
#endif
}

} // extern "C"

#if defined(BOOST_SYNC_DETAIL_TLS)

BOOST_FORCEINLINE tss_manager::thread_context* get_thread_context() BOOST_NOEXCEPT
{
    return tss_context;
}
BOOST_FORCEINLINE void set_thread_context(tss_manager::thread_context* p) BOOST_NOEXCEPT
{
    tss_context = p;
}

#else // defined(BOOST_SYNC_DETAIL_TLS)

BOOST_FORCEINLINE tss_manager::thread_context* get_thread_context() BOOST_NOEXCEPT
{
    return static_cast< tss_manager::thread_context* >(pthread_getspecific(tss_key));
}
BOOST_FORCEINLINE void set_thread_context(tss_manager::thread_context* p) BOOST_NOEXCEPT
{
    pthread_setspecific(tss_key, p);
}

#endif // defined(BOOST_SYNC_DETAIL_TLS)

} // namespace

BOOST_SYNC_API void add_thread_exit_notify_entry(sync::mutex& mtx, sync::condition_variable& cond)
{
    pthread_once(&init_tss_once_flag, &init_tss);
    tss_manager::thread_context* ctx = get_thread_context();

    if (!ctx)
    {
        ctx = tss_mgr->create_thread_context();
        set_thread_context(ctx);
    }

    ctx->add_notify_at_exit_entry(&mtx, &cond);
}

BOOST_SYNC_API void add_thread_exit_callback(at_thread_exit_callback callback, void* context)
{
    pthread_once(&init_tss_once_flag, &init_tss);
    tss_manager::thread_context* ctx = get_thread_context();

    if (!ctx)
    {
        ctx = tss_mgr->create_thread_context();
        set_thread_context(ctx);
    }

    ctx->add_at_exit_entry(callback, context);
}

BOOST_SYNC_API thread_specific_key new_thread_specific_key(at_thread_exit_callback callback, bool cleanup_at_delete)
{
    pthread_once(&init_tss_once_flag, &init_tss);
    return tss_mgr->new_key(callback, cleanup_at_delete);
}

BOOST_SYNC_API void delete_thread_specific_key(thread_specific_key key) BOOST_NOEXCEPT
{
    try
    {
        tss_mgr->delete_key(key);
    }
    catch (...)
    {
        std::abort();
    }
}

BOOST_SYNC_API void* get_thread_specific(thread_specific_key key) BOOST_NOEXCEPT
{
    tss_manager::thread_context* ctx = get_thread_context();
    if (ctx)
        return ctx->get_value(key);
    return NULL;
}

BOOST_SYNC_API void set_thread_specific(thread_specific_key key, void* p)
{
    tss_manager::thread_context* ctx = get_thread_context();

    if (!ctx)
    {
        ctx = tss_mgr->create_thread_context();
        set_thread_context(ctx);
    }

    ctx->set_value(key, p);
}

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // !defined(BOOST_SYNC_DETAIL_PLATFORM_WINAPI)
