/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   exceptions/wait_error.hpp
 *
 * \brief  This header defines the \c wait_error exception.
 */

#ifndef BOOST_SYNC_EXCEPTIONS_WAIT_ERROR_HPP_INCLUDED_
#define BOOST_SYNC_EXCEPTIONS_WAIT_ERROR_HPP_INCLUDED_

#include <string>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/system_error.hpp>
#include <boost/sync/exceptions/runtime_exception.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

class BOOST_SYMBOL_VISIBLE wait_error :
    public runtime_exception
{
public:
    explicit wait_error(int sys_err = 0) : runtime_exception(sys_err, "boost::sync::wait_error")
    {
    }

    wait_error(int sys_err, const char* what) : runtime_exception(sys_err, what)
    {
    }

    wait_error(int sys_err, std::string const& what) : runtime_exception(sys_err, what)
    {
    }

    ~wait_error() BOOST_NOEXCEPT_OR_NOTHROW
    {
    }
};

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_EXCEPTIONS_WAIT_ERROR_HPP_INCLUDED_
