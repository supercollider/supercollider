//  Copyright (C) 2011 Tim Blechmann
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

#ifndef UTILITIES_NAMED_HASH_ENTRY_HPP
#define UTILITIES_NAMED_HASH_ENTRY_HPP

#include <string>

#include <boost/intrusive/unordered_set_hook.hpp>

#include "utils.hpp"

#ifdef __GNUC__
#define PURE __attribute__((pure))
#else
#define PURE /*__attribute__((pure))*/
#endif

namespace nova   {
namespace bi = boost::intrusive;

PURE inline bool strequal(const char * lhs, const char * rhs)
{
    for(;;++lhs, ++rhs) {
        if (*lhs == 0) {
            if (*rhs == 0)
                return true;
            else
                return false;
        }
        if (*rhs == 0)
            return false;
    }
}

struct c_string
{
private:
    const char * str_;

    static const char * duplicate_string(const char * str)
    {
        return duplicate_string(str, strlen(str));
    }

    static const char * duplicate_string(const char * str, std::size_t length)
    {
        assert(strlen(str) == length);

        length += 1; // for terminating \0
        char * string = malloc_aligned<char>(length);

        strncpy(string, str, length);
        return string;
    }

public:
    explicit c_string (const char * str):
        str_(duplicate_string(str))
    {}

    c_string (const char * str, std::size_t length):
        str_(duplicate_string(str, length))
    {}

    ~c_string (void)
    {
        free_aligned((void*)str_);
    }

    const char * c_str(void) const
    {
        return str_;
    }

    friend std::size_t hash_value(c_string const & value)
    {
        return string_hash(value.str_);
    }

    friend bool operator== (c_string const & lhs,
                            c_string const & rhs)
    {
        return strequal(lhs.str_, rhs.str_);
    }
};

class named_hash_entry:
    public bi::unordered_set_base_hook<>
{
    const c_string name_;

public:
    named_hash_entry(const char * name):
        name_(name)
    {}

    named_hash_entry(std::string const & name):
        name_(name.c_str(), name.size())
    {}

    const char * name(void) const
    {
        return name_.c_str();
    }

    friend std::size_t hash_value(named_hash_entry const & that)
    {
        return hash_value(that.name_);
    }

    std::size_t hash(void) const
    {
        return hash_value(*this);
    }

    friend bool operator== (named_hash_entry const & a,
                            named_hash_entry const & b)
    {
        return a.name_ == b.name_;
    }
};

struct named_hash_equal
{
    template<typename def>
    bool operator()(def const & lhs,
                    std::string const & rhs) const
    {
        return operator()(lhs.name(), rhs.c_str());
    }

    template<typename def>
    bool operator()(std::string const & lhs,
                    def const & rhs) const
    {
        return operator()(lhs.c_str(), rhs.name());
    }

    template<typename def>
    bool operator()(const char * lhs, def const & rhs) const
    {
        return operator()(lhs, rhs.name());
    }

    template<typename def>
    bool operator()(def const & lhs, const char * rhs) const
    {
        return operator()(lhs.name(), rhs);
    }

    bool operator()(const char * lhs, const char * rhs) const
    {
        return strequal(rhs, lhs);
    }
};

struct named_hash_hash
{
    template<typename def>
    std::size_t operator()(def const & arg)
    {
        return arg.hash();
    }

    std::size_t operator()(const char * str)
    {
        return string_hash(str);
    }

    std::size_t operator()(std::string const & str)
    {
        return string_hash(str.c_str());
    }
};

}

#undef PURE
#endif /* UTILITIES_NAMED_HASH_ENTRY_HPP */
