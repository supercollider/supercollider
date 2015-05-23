/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   exceptions.hpp
 *
 * \brief  This header includes all exception types.
 */

#ifndef BOOST_SYNC_EXCEPTIONS_HPP_INCLUDED_
#define BOOST_SYNC_EXCEPTIONS_HPP_INCLUDED_

#include <boost/sync/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <boost/sync/exceptions/runtime_exception.hpp>
#include <boost/sync/exceptions/resource_error.hpp>
#include <boost/sync/exceptions/lock_error.hpp>
#include <boost/sync/exceptions/wait_error.hpp>
#include <boost/sync/exceptions/overflow_error.hpp>

#endif // BOOST_SYNC_EXCEPTIONS_HPP_INCLUDED_
