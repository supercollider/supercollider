/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   detail/throw_exception.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_THROW_EXCEPTION_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_THROW_EXCEPTION_HPP_INCLUDED_

#include <boost/throw_exception.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/sync/detail/config.hpp>
#if defined(BOOST_EXCEPTION_DISABLE)
#include <boost/current_function.hpp>
#endif
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

template< typename Exception, typename... T >
BOOST_NOINLINE BOOST_NORETURN void throw_exception(const char* func, const char* file, int line, T&&... args)
{
#if !defined(BOOST_EXCEPTION_DISABLE)
    boost::throw_exception
    (
        set_info
        (
            set_info
            (
                set_info
                (
                    enable_error_info(Exception(static_cast< T&& >(args)...)),
                    throw_function(func)
                ),
                throw_file(file)
            ),
            throw_line(line)
        )
    );
#else
    boost::throw_exception(Exception(static_cast< T&& >(args)...));
#endif
}

#else // !defined(BOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

template< typename Exception >
BOOST_NOINLINE BOOST_NORETURN void throw_exception(const char* func, const char* file, int line)
{
#if !defined(BOOST_EXCEPTION_DISABLE)
    boost::throw_exception
    (
        set_info
        (
            set_info
            (
                set_info
                (
                    enable_error_info(Exception()),
                    throw_function(func)
                ),
                throw_file(file)
            ),
            throw_line(line)
        )
    );
#else
    boost::throw_exception(Exception());
#endif
}

template< typename Exception, typename T0 >
BOOST_NOINLINE BOOST_NORETURN void throw_exception(const char* func, const char* file, int line, T0 const& arg0)
{
#if !defined(BOOST_EXCEPTION_DISABLE)
    boost::throw_exception
    (
        set_info
        (
            set_info
            (
                set_info
                (
                    enable_error_info(Exception(arg0)),
                    throw_function(func)
                ),
                throw_file(file)
            ),
            throw_line(line)
        )
    );
#else
    boost::throw_exception(Exception(arg0));
#endif
}

template< typename Exception, typename T0, typename T1 >
BOOST_NOINLINE BOOST_NORETURN void throw_exception(const char* func, const char* file, int line, T0 const& arg0, T1 const& arg1)
{
#if !defined(BOOST_EXCEPTION_DISABLE)
    boost::throw_exception
    (
        set_info
        (
            set_info
            (
                set_info
                (
                    enable_error_info(Exception(arg0, arg1)),
                    throw_function(func)
                ),
                throw_file(file)
            ),
            throw_line(line)
        )
    );
#else
    boost::throw_exception(Exception(arg0, arg1));
#endif
}

template< typename Exception, typename T0, typename T1, typename T2 >
BOOST_NOINLINE BOOST_NORETURN void throw_exception(const char* func, const char* file, int line, T0 const& arg0, T1 const& arg1, T2 const& arg2)
{
#if !defined(BOOST_EXCEPTION_DISABLE)
    boost::throw_exception
    (
        set_info
        (
            set_info
            (
                set_info
                (
                    enable_error_info(Exception(arg0, arg1, arg2)),
                    throw_function(func)
                ),
                throw_file(file)
            ),
            throw_line(line)
        )
    );
#else
    boost::throw_exception(Exception(arg0, arg1, arg2));
#endif
}

#endif // !defined(BOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

#if !defined(BOOST_EXCEPTION_DISABLE)
#define BOOST_SYNC_DETAIL_HERE BOOST_THROW_EXCEPTION_CURRENT_FUNCTION, __FILE__, __LINE__
#else
#define BOOST_SYNC_DETAIL_HERE BOOST_CURRENT_FUNCTION, __FILE__, __LINE__
#endif

#define BOOST_SYNC_DETAIL_THROW_DEFAULT(ex)\
    boost::sync::detail::throw_exception< ex >(BOOST_SYNC_DETAIL_HERE)

#define BOOST_SYNC_DETAIL_THROW(ex, args_seq)\
    boost::sync::detail::throw_exception< ex >(BOOST_SYNC_DETAIL_HERE, BOOST_PP_SEQ_ENUM(args_seq))

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_THROW_EXCEPTION_HPP_INCLUDED_
