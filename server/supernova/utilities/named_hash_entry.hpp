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

namespace nova   {
namespace bi = boost::intrusive;

class named_hash_entry:
    public bi::unordered_set_base_hook<>
{
    const std::string name_;

public:
    named_hash_entry(const char * name):
        name_(name)
    {}

    named_hash_entry(std::string const & name):
        name_(name)
    {}

    std::string const & name(void) const
    {
        return name_;
    }

    friend std::size_t hash_value(named_hash_entry const & that)
    {
        return string_hash(that.name().c_str());
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
        return lhs.name() == rhs;
    }

    template<typename def>
    bool operator()(std::string const & lhs,
                    def const & rhs) const
    {
        return lhs == rhs.name();
    }

    template<typename def>
    bool operator()(const char * lhs, def const & rhs) const
    {
        const char * rhstr = rhs.name().c_str();

        for(;;++lhs, ++rhstr)
        {
            if (*lhs == 0) {
                if (*rhstr == 0)
                    return true;
                else
                    return false;
            }
            if (*rhstr == 0)
                return false;
        }
    }

    template<typename def>
    bool operator()(def const & lhs, const char * rhs) const
    {
        return operator()(rhs, lhs);
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

#endif /* UTILITIES_NAMED_HASH_ENTRY_HPP */
