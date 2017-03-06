/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   tss_manager.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_IMPL_TSS_MANAGER_HPP_INCLUDED_
#define BOOST_SYNC_IMPL_TSS_MANAGER_HPP_INCLUDED_

#include <stack>
#include <vector>
#include <boost/assert.hpp>
#include <boost/intrusive/options.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/list_hook.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/locks/lock_guard.hpp>
#include <boost/sync/locks/unique_lock.hpp>
#include <boost/sync/mutexes/mutex.hpp>
#include <boost/sync/condition_variables/condition_variable.hpp>
#include <boost/sync/detail/tss.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

class tss_manager
{
public:
    typedef intrusive::list_base_hook<
        intrusive::tag< struct for_thread_context_list >,
        intrusive::link_mode< intrusive::safe_link >
    > thread_context_list_hook;

    class thread_context :
        public thread_context_list_hook
    {
        friend class tss_manager;

    private:
        struct at_exit_entry
        {
            at_thread_exit_callback callback;
            void* context;
        };

        struct notify_at_exit_entry
        {
            sync::mutex* mtx;
            sync::condition_variable* cond;
        };

    private:
        std::vector< void* > m_storage;
        std::vector< at_exit_entry > m_at_exit_functions;
        std::vector< notify_at_exit_entry > m_notify_at_exit;

    public:
        void* get_value(thread_specific_key key) const BOOST_NOEXCEPT
        {
            if (key < m_storage.size())
                return m_storage[key];
            else
                return NULL;
        }

        void set_value(thread_specific_key key, void* value)
        {
            if (key >= m_storage.size())
                m_storage.resize(key + 1, static_cast< void* >(NULL));
            m_storage[key] = value;
        }

        void add_at_exit_entry(at_thread_exit_callback callback, void* context)
        {
            at_exit_entry entry;
            entry.callback = callback;
            entry.context = context;
            m_at_exit_functions.push_back(entry);
        }

        void add_notify_at_exit_entry(sync::mutex* mtx, sync::condition_variable* cond)
        {
            notify_at_exit_entry entry;
            entry.mtx = mtx;
            entry.cond = cond;
            m_notify_at_exit.push_back(entry);
        }
    };

private:
    typedef sync::mutex mutex_type;

    typedef intrusive::list<
        thread_context,
        intrusive::base_hook< thread_context_list_hook >,
        intrusive::constant_time_size< false >
    > thread_context_list;

    struct cleanup_info
    {
        at_thread_exit_callback cleanup;
        bool cleanup_at_delete;
    };

private:
    //! Thread synchronization primitive
    mutex_type m_mutex;
    //! The list of thread contexts
    thread_context_list m_thread_contexts;
    //! The list of cleanup functions for TLS
    std::vector< cleanup_info > m_storage_cleanup;
    //! The list of TLS keys released
    std::stack< thread_specific_key > m_freed_keys;

public:
    BOOST_DEFAULTED_FUNCTION(tss_manager(), {})

    ~tss_manager()
    {
        while (!m_thread_contexts.empty())
            destroy_thread_context(&m_thread_contexts.front());
    }

    thread_context* create_thread_context()
    {
        thread_context* p = new thread_context();

        sync::lock_guard< mutex_type > lock(m_mutex);
        m_thread_contexts.push_back(*p);

        return p;
    }

    void destroy_thread_context(thread_context* p) BOOST_NOEXCEPT
    {
        // Run cleanup functions
        while (!p->m_at_exit_functions.empty())
        {
            std::vector< thread_context::at_exit_entry > at_exit_functions;
            at_exit_functions.swap(p->m_at_exit_functions);

            for (std::vector< thread_context::at_exit_entry >::reverse_iterator it = at_exit_functions.rbegin(), end = at_exit_functions.rend(); it != end; ++it)
            {
                it->callback(it->context);
            }
        }

        // Destroy TLS
        while (!p->m_storage.empty())
        {
            std::vector< void* > storage;
            storage.swap(p->m_storage);
            for (thread_specific_key key = 0, n = static_cast< thread_specific_key >(storage.size()); key < n; ++key)
            {
                void* const value = storage[key];
                if (value)
                {
                    const at_thread_exit_callback cleanup = m_storage_cleanup[key].cleanup;
                    if (cleanup)
                        cleanup(value);
                }
            }
        }

        // Notify about thread termination. This must be performed last, after all TLS variables are destroyed.
        for (std::vector< thread_context::notify_at_exit_entry >::const_iterator it = p->m_notify_at_exit.begin(), end = p->m_notify_at_exit.end(); it != end; ++it)
        {
            it->mtx->unlock();
            it->cond->notify_all();
        }

        // Destroy the context
        {
            sync::lock_guard< mutex_type > lock(m_mutex);
            m_thread_contexts.erase(m_thread_contexts.iterator_to(*p));
        }
        delete p;
    }

    thread_specific_key new_key(at_thread_exit_callback cleanup, bool cleanup_at_delete)
    {
        sync::lock_guard< mutex_type > lock(m_mutex);

        // See if we can recycle some key
        thread_specific_key key;
        if (!m_freed_keys.empty())
        {
            key = m_freed_keys.top();
            m_freed_keys.pop();
            cleanup_info& info = m_storage_cleanup[key];
            info.cleanup = cleanup;
            info.cleanup_at_delete = cleanup_at_delete;
        }
        else
        {
            key = static_cast< thread_specific_key >(m_storage_cleanup.size());
            cleanup_info info;
            info.cleanup = cleanup;
            info.cleanup_at_delete = cleanup_at_delete;
            m_storage_cleanup.push_back(info);
        }

        return key;
    }

    void delete_key(thread_specific_key key)
    {
        sync::unique_lock< mutex_type > lock(m_mutex);

        BOOST_ASSERT(key < m_storage_cleanup.size());

        cleanup_info& info = m_storage_cleanup[key];
        if (info.cleanup_at_delete)
        {
            std::vector< void* > storage;
            const at_thread_exit_callback cleanup = info.cleanup;
            info.cleanup = NULL;

            for (thread_context_list::iterator it = m_thread_contexts.begin(), end = m_thread_contexts.end(); it != end; ++it)
            {
                if (it->m_storage.size() > key && it->m_storage[key] != NULL)
                {
                    if (cleanup)
                        storage.push_back(it->m_storage[key]);
                    it->m_storage[key] = NULL;
                }
            }

            m_freed_keys.push(key);

            lock.unlock();

            // Run cleanup routines while the lock is released
            for (std::vector< void* >::iterator it = storage.begin(), end = storage.end(); it != end; ++it)
            {
                cleanup(*it);
            }
        }
    }

    BOOST_DELETED_FUNCTION(tss_manager(tss_manager const&))
    BOOST_DELETED_FUNCTION(tss_manager& operator= (tss_manager const&))
};

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_IMPL_TSS_MANAGER_HPP_INCLUDED_
