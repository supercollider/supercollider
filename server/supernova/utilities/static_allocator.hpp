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

#include <array>
#include <stdexcept>

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
    static constexpr std::size_t bytes = 2 * count * sizeof(T) + 4096 * 2;

    static constexpr std::size_t alignment = sizeof(std::size_t);

    static_assert(bytes % alignment == 0, "static_allocator: bytes not an integer mutiple of alignment");

    using mutex_type = std::conditional_t<blocking, spin_lock, dummy_mutex>;

    using scoped_lock = std::lock_guard<mutex_type>;

    struct data : mutex_type {
#if 0
        data() {}

        data(data const & rhs)
        {
            pool = rhs.pool;
        }
#endif

        alignas(alignment) std::array<char, bytes> pool;
    };

public:
    typedef T value_type;

    static_allocator() noexcept {
        data_.pool.assign(0);
        init_memory_pool(bytes, data_.pool.data());
    }

    static_allocator(static_allocator const&) = delete;

#if 0
    template <class U, std::size_t bytes_, bool b>
    static_allocator(static_allocator<U, bytes_, b> const & rhs) noexcept {
        data_.pool.assign(0);
        init_memory_pool(bytes, data_.pool.data());
    }
#endif

    ~static_allocator() { destroy_memory_pool(data_.pool.data()); }

    [[nodiscard]] value_type* allocate(size_t n) {
        scoped_lock lock(data_);
        value_type* ret = static_cast<value_type*>(malloc_ex(n * sizeof(T), data_.pool.data()));

        if (ret == 0)
            throw std::bad_alloc();

        return ret;
    }

    void deallocate(value_type* p, size_t n) {
        scoped_lock lock(data_);
        free_ex(p, data_.pool.data());
    }

    size_t max_size() const noexcept { return count; }

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
