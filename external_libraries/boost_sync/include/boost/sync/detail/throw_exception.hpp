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
#include <boost/assert/source_location.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

template< typename Exception, typename... T >
BOOST_NOINLINE BOOST_NORETURN void throw_exception(boost::source_location const& loc, T&&... args)
{
    boost::throw_exception(Exception(static_cast< T&& >(args)...), loc);
}

#else // !defined(BOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

template< typename Exception >
BOOST_NOINLINE BOOST_NORETURN void throw_exception(boost::source_location const& loc)
{
    boost::throw_exception(Exception(), loc);
}

template< typename Exception, typename T0 >
BOOST_NOINLINE BOOST_NORETURN void throw_exception(boost::source_location const& loc, T0 const& arg0)
{
    boost::throw_exception(Exception(arg0), loc);
}

template< typename Exception, typename T0, typename T1 >
BOOST_NOINLINE BOOST_NORETURN void throw_exception(boost::source_location const& loc, T0 const& arg0, T1 const& arg1)
{
    boost::throw_exception(Exception(arg0, arg1), loc);
}

template< typename Exception, typename T0, typename T1, typename T2 >
BOOST_NOINLINE BOOST_NORETURN void throw_exception(boost::source_location const& loc, int line, T0 const& arg0, T1 const& arg1, T2 const& arg2)
{
    boost::throw_exception(Exception(arg0, arg1, arg2), loc);
}

#endif // !defined(BOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

#define BOOST_SYNC_DETAIL_THROW_DEFAULT(ex)\
    boost::sync::detail::throw_exception< ex >(BOOST_CURRENT_LOCATION)

#define BOOST_SYNC_DETAIL_THROW(ex, args_seq)\
    boost::sync::detail::throw_exception< ex >(BOOST_CURRENT_LOCATION, BOOST_PP_SEQ_ENUM(args_seq))

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_THROW_EXCEPTION_HPP_INCLUDED_
