/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef SC_ASSERT_H
#define SC_ASSERT_H

#ifndef ANALYZER_NORETURN
#    ifdef __clang__
#        if __has_feature(attribute_analyzer_noreturn)
#            define ANALYZER_NORETURN __attribute__((analyzer_noreturn))
#        else
#            define ANALYZER_NORETURN
#        endif
#    else
#        define ANALYZER_NORETURN
#    endif
#endif

#define ASSERTS_ARE_FATAL 0
#define BOOST_ENABLE_ASSERT_DEBUG_HANDLER

#include <stdio.h>
#include <signal.h>
#include <cassert>
#include <boost/assert.hpp>

#define SC_ASSERT(expr) BOOST_ASSERT(expr)
#define SC_ASSERT_MSG(expr) BOOST_ASSERT_MSG(expr)
#define SC_VERIFY(expr) BOOST_VERIFY(expr)
#define SC_VERIFY_MSG(expr) BOOST_VERIFY_MSG(expr)

// SC_COND_ASSERT behaves like SC_ASSERT except that it is always executed, and returns the boolean value of it's
// expression.
//
#if defined(BOOST_DISABLE_ASSERTS) || (defined(BOOST_ENABLE_ASSERT_DEBUG_HANDLER) && defined(NDEBUG))
#    define SC_COND_ASSERT(expr) (expr)
#else
#    define SC_COND_ASSERT(expr)                                                                                       \
        ((expr) ? (true) : (::boost::assertion_failed(#expr, BOOST_CURRENT_FUNCTION, __FILE__, __LINE__), false))
#endif

#ifdef _MSC_VER
#    define SC_BREAK                                                                                                   \
        { __debugbreak(); }
#else
#    define SC_BREAK                                                                                                   \
        { raise(SIGTRAP); }
#endif


namespace boost {
void assertion_failed(char const* expr, char const* function, char const* file, long line) ANALYZER_NORETURN;
void assertion_failed_msg(char const* expr, char const* msg, char const* function, char const* file,
                          long line) ANALYZER_NORETURN;
}

#endif