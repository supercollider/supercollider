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
    typedef typename boost::mpl::if_c<blocking, spin_lock, dummy_mutex>::type mutex_type;

    typedef typename mutex_type::scoped_lock scoped_lock;

    struct data : mutex_type {
        data(void): pool(nullptr) {}

        void init(std::size_t size, bool lock) {
            pool = (char*)operator new(size);
            mlock(pool, size);

            std::memset(pool, 0, size);
            init_memory_pool(size, pool);
        }

        ~data(void) {
            scoped_lock lock(*this);
            if (pool) {
                destroy_memory_pool(pool);
                operator delete(pool);
            }
        }

        char* pool;
    };

public:
    simple_pool(void) {}

    simple_pool(simple_pool const& rhs) = delete;
    simple_pool& operator=(simple_pool const& rhs) = delete;

    simple_pool(std::size_t size, bool lock = false) { init(size, lock); }

    void init(std::size_t size, bool lock = false) {
#ifndef NOVA_MEMORY_DEBUGGING
        assert(size % sizeof(long) == 0);
        data_.init(size, lock);
#endif
    }

    ~simple_pool() throw() {}

#ifdef NOVA_MEMORY_DEBUGGING
    void* MALLOC malloc(std::size_t size) { return ::malloc(size); }

    void* MALLOC realloc(void* p, std::size_t size) { return ::realloc(p, size); }

    void free(void* p) { return ::free(p); }

    std::size_t get_max_size(void) { return std::numeric_limits<std::size_t>::max(); }
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

    std::size_t get_max_size(void) { return ::get_max_size(data_.pool); }
#endif
private:
    data data_;
};

} /* namespace nova */
