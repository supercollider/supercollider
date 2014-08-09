/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   waitable_timer.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_WAITABLE_TIMER_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_WAITABLE_TIMER_HPP_INCLUDED_

#include <winerror.h>
#include <limits.h>
#include <cstddef>
#include <cstdlib>
#include <stdexcept>
#include <boost/cstdint.hpp>
#include <boost/version.hpp>
#include <boost/detail/winapi/dll.hpp>
#include <boost/detail/winapi/tls.hpp>
#include <boost/detail/winapi/process.hpp>
#include <boost/detail/winapi/handles.hpp>
#include <boost/detail/winapi/thread.hpp>
#include <boost/detail/winapi/thread_pool.hpp>
#include <boost/detail/winapi/waitable_timer.hpp>
#include <boost/detail/winapi/synchronization.hpp>
#include <boost/detail/winapi/GetLastError.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/interlocked.hpp>
#include <boost/sync/detail/weak_linkage.hpp>
#include <boost/sync/detail/throw_exception.hpp>
#include <boost/sync/exceptions/resource_error.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

namespace windows {

/*!
 * \brief Global waitable timer state.
 *
 * The implementation ensures that every thread that requests a waitable timer receives its own timer handle. The handle
 * is created only once per thread and is kept in the thread-local storage. On the first request, the storage is initialized
 * with a pointer to \c thread_local_context, and a wait job is registered so that the context is deleted when the requesting thread
 * terminates.
 *
 * Furthermore, in order to make sure the TLS is process-wide (not just specific to a current module), we allocate a process-specific
 * semaphore and store the TLS key as its counter. This allows to allocate only one TLS slot for the whole process.
 *
 * The code for getting the semaphore counter is based on Boost.Interprocess by Ion Gaztanaga and the following link:
 * http://undocumented.ntinternals.net/UserMode/Undocumented%20Functions/NT%20Objects/Semaphore/NtQuerySemaphore.html
 */
struct waitable_timer_state
{
    enum init_state
    {
        st_uninitialized = 0,
        st_in_progress,
        st_initialized
    };

    long initialized;
    boost::detail::winapi::DWORD_ tls_key;
    boost::detail::winapi::HANDLE_ tls_key_holder;

    struct thread_local_context
    {
        const uint32_t boost_version; // for ABI compatibility control
        boost::detail::winapi::HANDLE_ waitable_timer;
        boost::detail::winapi::HANDLE_ current_thread;
        boost::detail::winapi::HANDLE_ wait_handle;

        BOOST_CONSTEXPR thread_local_context() BOOST_NOEXCEPT :
            boost_version(BOOST_VERSION),
            waitable_timer(NULL),
            current_thread(NULL),
            wait_handle(NULL)
        {
        }

        ~thread_local_context()
        {
            if (wait_handle)
                boost::detail::winapi::UnregisterWait(wait_handle);
            if (current_thread)
                boost::detail::winapi::CloseHandle(current_thread);
            if (waitable_timer)
                boost::detail::winapi::CloseHandle(waitable_timer);
        }

        static void NTAPI destroy(boost::detail::winapi::PVOID_ p, boost::detail::winapi::BOOLEAN_ /*timed_out*/)
        {
            delete static_cast< thread_local_context* >(p);
        }

        BOOST_DELETED_FUNCTION(thread_local_context(thread_local_context const&))
        BOOST_DELETED_FUNCTION(thread_local_context& operator= (thread_local_context const&))
    };

    struct semaphore_basic_information
    {
        boost::detail::winapi::ULONG_ current_count; // current semaphore count
        boost::detail::winapi::ULONG_ maximum_count; // max semaphore count
    };

    //! Initializes the process-wide TLS slot
    void init()
    {
        while (true)
        {
            long old_val = BOOST_ATOMIC_INTERLOCKED_COMPARE_EXCHANGE(&initialized, st_in_progress, st_uninitialized);
            if (old_val == st_in_progress)
            {
                // Wait for another thread
                boost::detail::winapi::SwitchToThread();
            }
            else if (old_val == st_initialized)
            {
                // Another thread completed the initialization
                return;
            }
            else
                break;
        }

        // Compose a process-specific semaphore name
#ifndef BOOST_NO_ANSI_APIS
        char sem_name[24] =
#else
        wchar_t sem_name[24] =
#endif
        {
            'b', 'o', 'o', 's', 't', '_', 's', 'y', 'n', 'c', '_', 't', 'l', 's', '_'
        };
        boost::detail::winapi::DWORD_ process_id = boost::detail::winapi::GetCurrentProcessId();
        for (unsigned int i = 0; i < 4; ++i)
        {
            uint8_t b = static_cast< uint8_t >(process_id >> ((3u - i) * 8u));
            uint8_t half = b >> 4;
            if (half < 10)
                sem_name[15 + i * 2] = '0' + half;
            else
                sem_name[15 + i * 2] = 'a' - 10 + half;

            half = b & 0x0f;
            if (half < 10)
                sem_name[16 + i * 2] = '0' + half;
            else
                sem_name[16 + i * 2] = 'a' - 10 + half;
        }
        sem_name[23] = '\0';

        // First try if the semaphore has already been created
#ifndef BOOST_NO_ANSI_APIS
        tls_key_holder = boost::detail::winapi::OpenSemaphoreA
#else
        tls_key_holder = boost::detail::winapi::OpenSemaphoreW
#endif
        (
            0x001F0003, // SEMAPHORE_ALL_ACCESS
            false,
            sem_name
        );

        if (!tls_key_holder)
        {
            // The semaphore is not created yet
            boost::detail::winapi::DWORD_ key = boost::detail::winapi::TlsAlloc();
            if (key == boost::detail::winapi::tls_out_of_indexes)
            {
                boost::detail::winapi::DWORD_ err = boost::detail::winapi::GetLastError();
                BOOST_ATOMIC_INTERLOCKED_EXCHANGE(&initialized, st_uninitialized);
                BOOST_SYNC_DETAIL_THROW(resource_error, (err)("Boost.Sync: unable to allocate a TLS slot"));
            }

#ifndef BOOST_NO_ANSI_APIS
            tls_key_holder = boost::detail::winapi::CreateSemaphoreA
#else
            tls_key_holder = boost::detail::winapi::CreateSemaphoreW
#endif
            (
                NULL, key, LONG_MAX, sem_name
            );

            boost::detail::winapi::DWORD_ err = boost::detail::winapi::GetLastError();
            if (!tls_key_holder)
            {
                // Cannot create a semaphore. Too bad, this will cause TLS slots to be allocated for every module.
                tls_key = key;
                BOOST_ATOMIC_INTERLOCKED_EXCHANGE(&initialized, st_initialized);
                return;
            }
            else if (err == ERROR_ALREADY_EXISTS)
            {
                // Some other thread managed to create the semaphore
                boost::detail::winapi::TlsFree(key);
            }
            else
            {
                tls_key = key;
                std::atexit(&waitable_timer_state::destroy);
                BOOST_ATOMIC_INTERLOCKED_EXCHANGE(&initialized, st_initialized);
                return;
            }
        }

        // Release the semaphore on process exit
        std::atexit(&waitable_timer_state::destroy);

        typedef boost::detail::winapi::DWORD_ NTSTATUS_;
        typedef NTSTATUS_ (__stdcall *NtQuerySemaphore_t)(boost::detail::winapi::HANDLE_ h, unsigned int info_class, semaphore_basic_information* pinfo, boost::detail::winapi::ULONG_ info_size, boost::detail::winapi::ULONG_* ret_len);

        // Retrieve the TLS key from the semaphore
        const boost::detail::winapi::HMODULE_ ntdll =
#ifndef BOOST_NO_ANSI_APIS
            boost::detail::winapi::GetModuleHandleA("ntdll.dll");
#else
            boost::detail::winapi::GetModuleHandleW(L"ntdll.dll");
#endif
        NtQuerySemaphore_t nt_query_semaphore = (NtQuerySemaphore_t)boost::detail::winapi::GetProcAddress(ntdll, "NtQuerySemaphore");
        if (nt_query_semaphore)
        {
            semaphore_basic_information info = {};
            NTSTATUS_ err = nt_query_semaphore(tls_key_holder, 0 /* SemaphoreBasicInformation */, &info, sizeof(info), NULL);
            if (err == 0)
            {
                tls_key = static_cast< boost::detail::winapi::DWORD_ >(info.current_count);
                BOOST_ATOMIC_INTERLOCKED_EXCHANGE(&initialized, st_initialized);
                return;
            }
        }

        // We failed to obtain the TLS key from the semaphore. Just allocate one already.
        boost::detail::winapi::DWORD_ key = boost::detail::winapi::TlsAlloc();
        if (key == boost::detail::winapi::tls_out_of_indexes)
        {
            boost::detail::winapi::DWORD_ err = boost::detail::winapi::GetLastError();
            BOOST_ATOMIC_INTERLOCKED_EXCHANGE(&initialized, st_uninitialized);
            BOOST_SYNC_DETAIL_THROW(resource_error, (err)("Boost.Sync: unable to allocate a TLS slot"));
        }

        tls_key = key;
        BOOST_ATOMIC_INTERLOCKED_EXCHANGE(&initialized, st_initialized);
    }

    //! Initializes the thread-specific context
    thread_local_context* create_thread_local_context()
    {
        thread_local_context* ctx = new thread_local_context();
        const boost::detail::winapi::HANDLE_ current_process = boost::detail::winapi::GetCurrentProcess();
        boost::detail::winapi::BOOL_ res = boost::detail::winapi::DuplicateHandle
        (
            current_process,
            boost::detail::winapi::GetCurrentThread(),
            current_process,
            &ctx->current_thread,
            0,
            false,
            boost::detail::winapi::duplicate_same_access
        );
        if (res)
        {
            res = boost::detail::winapi::RegisterWaitForSingleObject
            (
                &ctx->wait_handle,
                ctx->current_thread,
                &thread_local_context::destroy,
                ctx,
                boost::detail::winapi::infinite,
                boost::detail::winapi::wt_execute_in_wait_thread | boost::detail::winapi::wt_execute_only_once
            );
            if (res)
            {
                ctx->waitable_timer = boost::detail::winapi::create_anonymous_waitable_timer(NULL, false);
                if (ctx->waitable_timer)
                {
                    res = boost::detail::winapi::TlsSetValue(tls_key, ctx);
                    if (res)
                        return ctx;
                }
            }
        }

        boost::detail::winapi::DWORD_ err = boost::detail::winapi::GetLastError();
        delete ctx;
        BOOST_SYNC_DETAIL_THROW(resource_error, (err)("Boost.Sync: failed to initialize a waitable timer"));
        return NULL; // unreachable; to avoid warnings about missing return statement
    }

    //! Releases the TLS key holder semaphore handle
    static void destroy()
    {
        waitable_timer_state& state = weak_linkage< waitable_timer_state >::value;
        if (state.tls_key_holder)
        {
            boost::detail::winapi::CloseHandle(state.tls_key_holder);
            state.tls_key_holder = NULL;
        }
    }
};

//! Returns a thread-specific handle for a waitable timer for fixed time point waits
inline boost::detail::winapi::HANDLE_ get_waitable_timer()
{
    waitable_timer_state& state = weak_linkage< waitable_timer_state >::value;

    if (state.initialized != waitable_timer_state::st_initialized)
        state.init();

    waitable_timer_state::thread_local_context* p =
        static_cast< waitable_timer_state::thread_local_context* >(boost::detail::winapi::TlsGetValue(state.tls_key));
    if (!p)
        p = state.create_thread_local_context();

    // Check that the thread local context is ABI-compatible
    if (p->boost_version != BOOST_VERSION)
        BOOST_SYNC_DETAIL_THROW(std::logic_error, ("Boost.Sync: different Boost versions are used in the application"));

    return p->waitable_timer;
}

} // namespace windows

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_WAITABLE_TIMER_HPP_INCLUDED_
