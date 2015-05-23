/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2005-2007 Anthony Williams
 * (C) Copyright 2007 David Deakins
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   auto_handle.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 *
 * This header contains a scope guard to automatically release Windows handles.
 */

#ifndef BOOST_SYNC_DETAIL_AUTO_HANDLE_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_AUTO_HANDLE_HPP_INCLUDED_

#include <cstddef>
#include <boost/assert.hpp>
#include <boost/detail/winapi/handles.hpp>
#include <boost/detail/winapi/GetLastError.hpp>
#include <boost/detail/winapi/GetCurrentProcess.hpp>
#include <boost/sync/detail/config.hpp>
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

class auto_handle
{
private:
    boost::detail::winapi::HANDLE_ m_handle;

public:
    BOOST_CONSTEXPR explicit auto_handle(boost::detail::winapi::HANDLE_ h = NULL) BOOST_NOEXCEPT : m_handle(h)
    {
    }

    ~auto_handle()
    {
        reset();
    }

    void reset(boost::detail::winapi::HANDLE_ h = NULL) BOOST_NOEXCEPT
    {
        if (m_handle && m_handle != boost::detail::winapi::invalid_handle_value)
        {
            BOOST_VERIFY(boost::detail::winapi::CloseHandle(m_handle));
        }
        m_handle = h;
    }

    boost::detail::winapi::HANDLE_ get() const BOOST_NOEXCEPT
    {
        return m_handle;
    }

    operator boost::detail::winapi::HANDLE_ () const BOOST_NOEXCEPT
    {
        return m_handle;
    }

    bool operator! () const BOOST_NOEXCEPT
    {
        return !m_handle;
    }

    boost::detail::winapi::HANDLE_ release() BOOST_NOEXCEPT
    {
        const boost::detail::winapi::HANDLE_ h = m_handle;
        m_handle = NULL;
        return h;
    }

    boost::detail::winapi::HANDLE_ duplicate() const
    {
        const boost::detail::winapi::HANDLE_ current_process = boost::detail::winapi::GetCurrentProcess();
        boost::detail::winapi::HANDLE_ new_handle = NULL;
        if (boost::detail::winapi::DuplicateHandle(current_process, m_handle, current_process, &new_handle, 0, false, boost::detail::winapi::duplicate_same_access) == 0)
        {
            const boost::detail::winapi::DWORD_ err = boost::detail::winapi::GetLastError();
            BOOST_SYNC_DETAIL_THROW(resource_error, (err)("boost::sync: failed to duplicate a handle"));
        }
        return new_handle;
    }

    void swap(auto_handle& that)
    {
        const boost::detail::winapi::HANDLE_ h = m_handle;
        m_handle = that.m_handle;
        that.m_handle = h;
    }

    BOOST_DELETED_FUNCTION(auto_handle(auto_handle const&))
    BOOST_DELETED_FUNCTION(auto_handle& operator= (auto_handle const&))
};

} // namespace windows

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_AUTO_HANDLE_HPP_INCLUDED_
