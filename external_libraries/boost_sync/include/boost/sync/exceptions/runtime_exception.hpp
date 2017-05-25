/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   exceptions/runtime_exception.hpp
 *
 * \brief  This header defines a base class for all exceptions from Boost.Sync.
 */

#ifndef BOOST_SYNC_EXCEPTIONS_RUNTIME_EXCEPTION_HPP_INCLUDED_
#define BOOST_SYNC_EXCEPTIONS_RUNTIME_EXCEPTION_HPP_INCLUDED_

#include <string>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/system_error.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

class BOOST_SYMBOL_VISIBLE runtime_exception :
    public sync::detail::system_ns::system_error
{
public:
    explicit runtime_exception(int sys_err = 0) : sync::detail::system_ns::system_error(sys_err, sync::detail::system_ns::system_category())
    {
    }

    runtime_exception(int sys_err, const char* what) :
        sync::detail::system_ns::system_error(sync::detail::system_ns::error_code(sys_err, sync::detail::system_ns::system_category()), what)
    {
    }

    runtime_exception(int sys_err, std::string const& what) :
        sync::detail::system_ns::system_error(sync::detail::system_ns::error_code(sys_err, sync::detail::system_ns::system_category()), what)
    {
    }

    ~runtime_exception() BOOST_NOEXCEPT_OR_NOTHROW
    {
    }

    int native_error() const BOOST_NOEXCEPT
    {
        return this->code().value();
    }
};

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_EXCEPTIONS_RUNTIME_EXCEPTION_HPP_INCLUDED_
