/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2011 Vicente J. Botet Escriba
 */
/*!
 * \file   sync/condition_variables/cv_status.hpp
 *
 * \brief  This header defines \c cv_status enumeration.
 */

#ifndef BOOST_SYNC_CONDITION_VARIABLES_CV_STATUS_HPP_INCLUDED_
#define BOOST_SYNC_CONDITION_VARIABLES_CV_STATUS_HPP_INCLUDED_

#include <boost/detail/scoped_enum_emulation.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

//! Condition variable wait status
BOOST_SCOPED_ENUM_DECLARE_BEGIN(cv_status)
{
    no_timeout,  //!< The operation completed successfully before timeout
    timeout      //!< Timeout expired
}
BOOST_SCOPED_ENUM_DECLARE_END(cv_status)

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_CONDITION_VARIABLES_CV_STATUS_HPP_INCLUDED_
