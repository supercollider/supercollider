/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2007-2008 Anthony Williams
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   thread_specific/thread_specific_ptr.hpp
 *
 * \brief  This header defines \c thread_specific_ptr smart-pointer.
 */

#ifndef BOOST_SYNC_THREAD_SPECIFIC_THREAD_SPECIFIC_PTR_HPP_INCLUDED_
#define BOOST_SYNC_THREAD_SPECIFIC_THREAD_SPECIFIC_PTR_HPP_INCLUDED_

#include <boost/assert.hpp>
#include <boost/utility/explicit_operator_bool.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/tss.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

template< typename T >
class thread_specific_ptr
{
public:
    typedef T element_type;

private:
    const sync::detail::thread_specific_key m_key;
    const sync::detail::at_thread_exit_callback m_cleanup;

public:
    explicit thread_specific_ptr() :
        m_key(sync::detail::new_thread_specific_key(&thread_specific_ptr< T >::default_cleanup, true)),
        m_cleanup(&thread_specific_ptr< T >::default_cleanup)
    {
    }

    explicit thread_specific_ptr(void (*cleanup)(T*)) :
        m_key(sync::detail::new_thread_specific_key((sync::detail::at_thread_exit_callback)cleanup, true)),
        m_cleanup((sync::detail::at_thread_exit_callback)cleanup)
    {
    }

    ~thread_specific_ptr()
    {
        sync::detail::delete_thread_specific_key(m_key);
    }

    T* get() const BOOST_NOEXCEPT
    {
        return static_cast< T* >(sync::detail::get_thread_specific(m_key));
    }

    T* operator-> () const BOOST_NOEXCEPT
    {
        T* const p = get();
        BOOST_ASSERT(!!p);
        return p;
    }

    T& operator* () const BOOST_NOEXCEPT
    {
        T* const p = get();
        BOOST_ASSERT(!!p);
        return *p;
    }

    bool operator! () const BOOST_NOEXCEPT
    {
        return !get();
    }

    BOOST_EXPLICIT_OPERATOR_BOOL()

    T* release()
    {
        T* const p = get();
        if (p)
            sync::detail::set_thread_specific(m_key, 0);
        return p;
    }

    void reset(T* new_value = 0)
    {
        T* const old_value = get();
        if (old_value != new_value)
        {
            sync::detail::set_thread_specific(m_key, new_value);
            if (old_value && m_cleanup)
                m_cleanup(old_value);
        }
    }

    BOOST_DELETED_FUNCTION(thread_specific_ptr(thread_specific_ptr const&))
    BOOST_DELETED_FUNCTION(thread_specific_ptr& operator= (thread_specific_ptr const&))

private:
    static void default_cleanup(void* p)
    {
        delete static_cast< T* >(p);
    }
};

template< typename T >
inline T* get_pointer(thread_specific_ptr< T > const& ptr)
{
    return ptr.get();
}

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_THREAD_SPECIFIC_THREAD_SPECIFIC_PTR_HPP_INCLUDED_
