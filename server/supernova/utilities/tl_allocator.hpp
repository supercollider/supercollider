//  thread-local real-time safe allocator class
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
#include <cstring>
#include <array>

#include <boost/thread/tss.hpp>

#include "branch_hints.hpp"

namespace nova {

namespace detail {

#ifdef __GNUC__
template <std::size_t bytes> class tl_allocator {
    static_assert(bytes % sizeof(long) == 0, "tl_allocator: bytes not an integer mutiple of sizeof(long)");

public:
    static const std::size_t poolsize = bytes / sizeof(long);
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    tl_allocator(void) throw() {
        if (likely(initialized))
            return;

        memset(pool, bytes, 0);
        init_memory_pool(bytes, pool);
    }

    void* allocate(size_type n) {
        void* ret = malloc_ex(n, pool);

        if (ret == 0)
            throw std::bad_alloc();

        return ret;
    }

    void deallocate(void* p) { free_ex(p, pool); }

    size_type max_size() const throw() { return bytes; }

    static __thread long pool[poolsize];
    static __thread bool initialized;
};

template <std::size_t bytes> long __thread tl_allocator<bytes>::pool[];

template <std::size_t bytes> bool __thread tl_allocator<bytes>::initialized = false;

#else

template <std::size_t bytes> class tl_allocator {
    static_assert(bytes % sizeof(long) == 0, "tl_allocator: bytes not an integer mutiple of sizeof(long)");
    static const std::size_t poolsize = bytes / sizeof(long);

    struct pool_t {
        pool_t(void) {
            pool.assign(0);
            init_memory_pool(bytes, pool.begin());
        }

        ~pool_t(void) { destroy_memory_pool(pool.begin()); }

        std::array<long, poolsize> pool;
    };

public:
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    tl_allocator(void) throw() {
        if (likely(pool.get()))
            return;
        pool.reset(new pool_t());
    }

    tl_allocator(tl_allocator const& rhs) throw() {}

    void* allocate(size_type n) {
        void* ret = malloc_ex(n, pool->pool.begin());

        if (ret == 0)
            throw std::bad_alloc();

        return ret;
    }

    void deallocate(void* p) { free_ex(p, pool->pool.begin()); }

    size_type max_size() const throw() { return bytes; }

    typedef boost::thread_specific_ptr<pool_t> pool_ptr;
    static pool_ptr pool;
};

template <std::size_t bytes> typename tl_allocator<bytes>::pool_ptr tl_allocator<bytes>::pool;

#endif

}

/**
 *
 * realtime allocator:
 * allocates from a thread-local memory pool via the malloc implementation of tlsf
 *
 * it is not completely stl compliant
 * memory allocated from one instance can be deallocated from each other instance
 * of this class as long as it is done from the same thread
 *
 * */
template <typename T, std::size_t bytes = 8 * 1024 * 1024> class tl_allocator {
public:
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    template <class U> struct rebind { typedef tl_allocator<U, bytes> other; };

    tl_allocator(void) throw() {}

    template <class U, std::size_t bytes_> tl_allocator(tl_allocator<U, bytes_> const& rhs) throw() {}

    pointer address(reference x) const { return &x; }

    const_pointer address(const_reference x) const { return &x; }

    pointer allocate(size_type n, const_pointer hint = 0) {
        return static_cast<pointer>(allocator.allocate(n * sizeof(T)));
    }

    void deallocate(pointer p, size_type n) { allocator.deallocate(p); }

    size_type max_size() const throw() { return bytes; }

    void construct(pointer p, const T& val) { ::new (p) T(val); }

    void destroy(pointer p) { p->~T(); }

private:
    detail::tl_allocator<bytes> allocator;
};

} /* namespace nova */
