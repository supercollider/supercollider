/**
 *  \file SC_Codecvt.hpp
 *
 *  \brief Codepage conversion utilities.
 *
 *  This header provides convenience functions to convert strings and paths between UTF-8 and the OS's
 *  filesystem API encoding. On POSIX systems, this does nothing. On Windows, this converts between
 *  UTF-8 and UTF-16. On Windows, additional functions are provided to convert between UTF-8 std::string
 *  and UTF-16 std::wstring.
 *
 *  $Author: Brian Heim $
 *
 *  \version 1.1
 *
 *  $Date: 2017-05-14 $
 *
 *  $Contact: brianlheim@gmail.com $
 *
 *  Created on: 2017-05-14
 */
/*
 *  Copyright (C) Brian Heim, 2017. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA
 */

#ifndef SC_CODECVT_HPP_INCLUDED
#define SC_CODECVT_HPP_INCLUDED

#include <string> // string
#include <boost/filesystem/path.hpp> // path

#ifdef _WIN32
#    include <codecvt> // std::codecvt_utf8_utf16, utf16
#    include <locale>  // std::wstring_convert
#endif

/** \namespace SC_Codecvt
 *  \brief Namespace for codepage conversion functions. */
namespace SC_Codecvt {

// Windows helper functions. Only defined on Windows to avoid having
// to unnecessarily include <codecvt> and <locale>
#ifdef _WIN32

/** \brief Converts a UTF-8 char str to UTF-16 std::wstring
 *
 * This function is only defined on Windows, to avoid unnecessary header includes. */
inline std::wstring utf8_cstr_to_utf16_wstring(const char *s)
{
	return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> >().from_bytes(s);
}

/** \brief Converts a UTF-16 wchar_t str to UTF-8 std::string
 *
 * This function is only defined on Windows, to avoid unnecessary header includes. */
inline std::string utf16_wcstr_to_utf8_string(const wchar_t *s)
{
	return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> >().to_bytes(s);
}

#endif // _WIN32

/** \brief Converts a path to a UTF-8 encoded string.
 *
 * On POSIX platforms, this just converts using .string(). On Windows, uses 
 * conversion between UTF-16 and UTF-8. */
inline std::string path_to_utf8_str(const boost::filesystem::path& p)
{
#ifdef _WIN32
    return p.string(std::codecvt_utf8_utf16<wchar_t>());
#else
    return p.string();
#endif // _WIN32
}

/** \brief Converts a UTF-8 encoded string to a path.
 *
 * On POSIX platforms, this converts using the default constructor. On Windows,
 * uses conversion between UTF-16 and UTF-8. */
inline boost::filesystem::path utf8_str_to_path(const std::string& s)
{
#ifdef _WIN32
    return boost::filesystem::path(s, std::codecvt_utf8_utf16<wchar_t>());
#else
    return boost::filesystem::path(s);
#endif // _WIN32
}

/** \brief Converts a native filesystem-encoded string to a UTF-8 string. 
 *
 * On Windows, converts between UTF-16 and UTF-8. On POSIX systems, no-op. */
inline std::string utf8_to_native_str(const std::string& s)
{
#ifdef _WIN32
    // first to wide string (native format)
    std::wstring ws = utf8_cstr_to_utf16_wstring(s.c_str());

    // then to string (still native)
    std::wstring_convert<std::codecvt_utf16<wchar_t> > conv_16to16;
    std::string ret = conv_16to16.to_bytes(ws);
    return ret;
#else
    return s;
#endif // _WIN32
}

/** \brief Converts a UTF-8 string to a native filesystem-encoded string.
 *
 * On Windows, converts between UTF-16 and UTF-8. On POSIX systems, no-op. */
inline std::string native_to_utf8_str(const std::string& s)
{
#ifdef _WIN32
    // first to wide string (still native format)
    std::wstring_convert<std::codecvt_utf16<wchar_t> > conv_16to16;
    std::wstring ws = conv_16to16.from_bytes(s);

    // then to string (utf8)
    std::string ret = utf16_wcstr_to_utf8_string(ws.c_str());
    return ret;
#else
    return s;
#endif // _WIN32
}

} // SC_Codecvt

#endif // SC_CODECVT_HPP_INCLUDED
