//  utility functions
//  Copyright (C) 2005-2012  Tim Blechmann
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

#pragma once

#include <cstddef>
#include <cassert>

#include <type_traits>

#include <boost/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/detail/atomic_count.hpp>

#include "branch_hints.hpp"

typedef unsigned int uint;

#include "function_attributes.h"


namespace nova {

/* we're using some member of the boost namespace */
using boost::intrusive_ptr;

/* some basic math functions */
inline bool ispoweroftwo(int i) { return (i & (i - 1)) == 0; }


template <unsigned int n> struct is_power_of_two {
    static const bool val = (n % 2 == 0) && (is_power_of_two<(n >> 1)>::val);
};

template <> struct is_power_of_two<2> { static const bool val = true; };

inline int log2(int n) {
    if (unlikely(n <= 0))
        return (0);

#ifdef __GNUC__
    return sizeof(int) * __CHAR_BIT__ - 1 - __builtin_clz(n);
#else
    int r = -1;
    while (n) {
        r++;
        n >>= 1;
    }
    return r;
#endif
}

inline int nextpoweroftwo(int n) {
    n = n - 1;
    const uint bitspace = sizeof(int) * 8 / 2;
    for (uint i = 1; i != bitspace; i *= 2)
        n = n | (n >> i);

    return n + 1;
}

using std::size_t;

/** \brief base class for a callback function */
template <typename t> class runnable {
public:
    virtual ~runnable(void) = default;

    virtual t run(void) = 0;
};


template <class T> struct default_deleter {
    void operator()(T* t) { delete t; }
};

struct delayed_deleter {
    template <typename T> inline void operator()(T*);
};

struct checked_deleter {
    template <class T> void operator()(T* x) const { boost::checked_delete(x); }
};


template <typename deleter = checked_deleter> struct intrusive_refcountable : public deleter {
    intrusive_refcountable(void): use_count_(0) {}

    intrusive_refcountable(intrusive_refcountable const& rhs) = delete;
    intrusive_refcountable& operator=(intrusive_refcountable const& rhs) = delete;

    virtual ~intrusive_refcountable(void) = default;

    void add_ref(void) { ++use_count_; }

    void release(void) {
        if (--use_count_ == 0)
            deleter::operator()(this);
    }

    inline friend void intrusive_ptr_add_ref(intrusive_refcountable* p) { p->add_ref(); }

    inline friend void intrusive_ptr_release(intrusive_refcountable* p) { p->release(); }

    boost::detail::atomic_count use_count_;
};

template <class t, class compare = std::less<t>> struct compare_by_instance {
    bool operator()(const t* lhs, const t* rhs) {
        assert(lhs and rhs);
        compare cmp;
        return cmp(*lhs, *rhs);
    }
};


PURE inline std::size_t string_hash(const char* str) {
    std::size_t ret = 0;

    // sdbm hash ... later try another function!
    int c;
    while ((c = *str++))
        ret = c + (ret << 6) + (ret << 16) - ret;

    return ret;
}

struct linear_allocator {
    linear_allocator(char* chunk): chunk(chunk) {}

    template <typename T> T* alloc(int count = 1) {
        T* ret = reinterpret_cast<T*>(chunk);
        chunk += count * sizeof(T);
        return ret;
    }

private:
    char* chunk;
};

} /* namespace nova */
