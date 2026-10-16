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

#include <array>
#include <stdexcept>

#include "branch_hints.hpp"

namespace nova {

namespace detail {

template <std::size_t bytes> class tl_allocator {
    static constexpr size_t alignment = sizeof(long);
    static_assert(bytes % alignment == 0, "tl_allocator: bytes not an integer mutiple of alignment");

public:
    tl_allocator() noexcept {
        if (likely(initialized))
            return;

        memset(pool.data(), pool.size(), 0);
        init_memory_pool(pool.size(), pool.data());
    }

    [[nodiscard]] void* allocate(size_t n) {
        void* ret = malloc_ex(n, pool.data());

        if (ret == 0)
            throw std::bad_alloc();

        return ret;
    }

    void deallocate(void* p) { free_ex(p, pool.data()); }

    size_t max_size() const noexcept { return bytes; }

    alignas(alignment) inline static thread_local std::array<char, bytes> pool;
    inline static thread_local bool initialized = false;
};

} /* detail */

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
    tl_allocator() noexcept {}

    template <class U, std::size_t bytes_> tl_allocator(tl_allocator<U, bytes_> const& rhs) noexcept {}

    T* allocate(size_t n) { return static_cast<T*>(allocator.allocate(n * sizeof(T))); }

    void deallocate(T* p, size_t n) { allocator.deallocate(p); }

    size_t max_size() const noexcept { return bytes; }

private:
    detail::tl_allocator<bytes> allocator;
};

} /* namespace nova */
