//  simple pool class
//  Copyright (C) 2009, 2010 Tim Blechmann
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

#include <cstdlib>
#include <cstring> /* for std::memset */

#include "nova-tt/spin_lock.hpp"
#include "nova-tt/dummy_mutex.hpp"
#include "nova-tt/mlock.hpp"

#include "function_attributes.h"

namespace nova {

/**
 * simple memory pool class, based on tlsf.
 *
 * its size has to be set at runtime before it can be used.
 *
 * */
template <bool blocking = false> class simple_pool {
    using mutex_type = std::conditional_t<blocking, spin_lock, dummy_mutex>;

    using scoped_lock = std::lock_guard<mutex_type>;

    struct data : mutex_type {
        data() = default;

        void init(std::size_t size, bool lock) {
            pool = (char*)operator new(size);
            mlock(pool, size);

            std::memset(pool, 0, size);
            init_memory_pool(size, pool);
            pool_size = size;
        }

        ~data() {
            scoped_lock lock(*this);
            if (pool) {
                destroy_memory_pool(pool);
                operator delete(pool);
            }
        }

        char* pool = nullptr;
        size_t pool_size = 0;
    };


public:
    simple_pool() {}

    simple_pool(simple_pool const& rhs) = delete;
    simple_pool& operator=(simple_pool const& rhs) = delete;

    simple_pool(std::size_t size, bool lock = false) { init(size, lock); }

    void init(std::size_t size, bool lock = false) {
#ifndef NOVA_MEMORY_DEBUGGING
        assert(size % sizeof(size_t) == 0);
        data_.init(size, lock);
#endif
    }

    ~simple_pool() {}

#ifdef NOVA_MEMORY_DEBUGGING
    void* MALLOC malloc(std::size_t size) { return ::malloc(size); }

    void* MALLOC realloc(void* p, std::size_t size) { return ::realloc(p, size); }

    void free(void* p) { return ::free(p); }

    std::size_t get_max_size() const { return std::numeric_limits<std::size_t>::max(); }
#else
    void* MALLOC ASSUME_ALIGNED(32) malloc(std::size_t size) {
        scoped_lock lock(data_);
        return malloc_ex(size, data_.pool);
    }

    void* MALLOC ASSUME_ALIGNED(32) realloc(void* p, std::size_t size) {
        scoped_lock lock(data_);
        return realloc_ex(p, size, data_.pool);
    }

    void free(void* p) {
        scoped_lock lock(data_);
        free_ex(p, data_.pool);
    }

    std::size_t get_pool_size() const { return data_.pool_size; }
    std::size_t get_max_size() const { return ::get_max_size(data_.pool); }
    std::size_t get_used_size() const { return ::get_used_size(data_.pool); }
#endif
private:
    data data_;
};

} /* namespace nova */
