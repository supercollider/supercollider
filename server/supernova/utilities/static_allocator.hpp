//  static allocator class
//  Copyright (C) 2008 Tim Blechmann
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

extern "C" {
#include "tlsf.h"
}

#include <exception>

#include <array>

#include "nova-tt/spin_lock.hpp"
#include "nova-tt/dummy_mutex.hpp"

namespace nova {

/** constant-sized, pooled memory allocator based on TLSF allocator
 *
 *  \todo this violates the allocator requirement in 20.1.5, paragraph 4
 *        objects have to be freed using the same instance that was used
 *        to allocate them.
 *
 *  */
template <class T, std::size_t count, bool blocking = false> class static_allocator {
    static const std::size_t bytes = 2 * count * sizeof(T) + 4096 * 2;

    static_assert(bytes % sizeof(long) == 0, "static_allocator: bytes not an integer mutiple of sizeof(long)");

    static const std::size_t poolsize = bytes / sizeof(long);

    typedef typename boost::mpl::if_c<blocking, spin_lock, dummy_mutex>::type mutex_type;

    typedef typename mutex_type::scoped_lock scoped_lock;

    struct data : mutex_type {
#if 0
        data(void){}

        data(data const & rhs)
        {
            pool = rhs.pool;
        }
#endif

        std::array<long, poolsize> pool;
    };

public:
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    template <class U> struct rebind { typedef static_allocator<U, count, blocking> other; };

    static_allocator(void) throw() {
        data_.pool.assign(0);
        init_memory_pool(bytes, data_.pool.begin());
    }

    static_allocator(static_allocator const&) = delete;

#if 0
    template <class U, std::size_t bytes_, bool b>
    static_allocator(static_allocator<U, bytes_, b> const & rhs) throw()
    {
        data_.pool.assign(0);
        init_memory_pool(bytes, data_.pool.begin());
    }
#endif

    ~static_allocator() throw() { destroy_memory_pool(data_.pool.begin()); }

    pointer address(reference x) const { return &x; }

    const_pointer address(const_reference x) const { return &x; }

    pointer allocate(size_type n, const_pointer hint = 0) {
        scoped_lock lock(data_);
        pointer ret = static_cast<pointer>(malloc_ex(n * sizeof(T), data_.pool.begin()));

        if (ret == 0)
            throw std::bad_alloc();

        return ret;
    }

    void deallocate(pointer p, size_type n) {
        scoped_lock lock(data_);
        free_ex(p, data_.pool.begin());
    }

    size_type max_size() const throw() { return count; }

    void construct(pointer p, const T& val) { ::new (p) T(val); }

    void destroy(pointer p) { p->~T(); }

private:
    data data_;
};


template <typename T, std::size_t ts, bool tb, typename U, std::size_t us, bool ub>
bool operator==(static_allocator<T, ts, tb> const& left, static_allocator<U, us, ub> const& right) {
    return !(left != right);
}

template <typename T, std::size_t ts, bool tb, typename U, std::size_t us, bool ub>
bool operator!=(static_allocator<T, ts, tb> const& left, static_allocator<U, us, ub> const& right) {
    return true;
}


} /* namespace nova */
