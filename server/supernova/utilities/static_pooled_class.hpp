//  static pooled class
//  Copyright (C) 2009 Tim Blechmann
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

#include <boost/mpl/if.hpp>

#include "freelist.hpp"
#include "static_pool.hpp"

namespace nova {

/**
 * base class for a class, which uses a static memory pool for
 * memory allocation.
 *
 * memory is allocated from a static pool and freed to a lock-free
 * freelist, which is freed during the memory allocation routine.
 * this way, the derived class can be allocated and freed from
 * different threads, while all the access to the pool is done from
 * the allocating thread.
 *
 * \tparam pool_locking specifies the locking policy for the object
 *         pool (default: nonblocking)
 *
 * \tparam recover_count limits the number of objects to be disposed
 *         from the freelist to avoid (default: dispose all objects
 *         of freelist)
 *
 * \todo   we could allocate one word more and store the size of the
 *         chunk just before the object. if a disposed object is large
 *         enough for a request, it wouldn't need to be added to the
 *         memory pool.
 *
 * */
template <typename tag, std::size_t pool_size, bool pool_locking = false, unsigned int recover_count = 0>
class static_pooled_class {
protected:
    static_pooled_class(void) = default;
    static_pooled_class(static_pooled_class const& rhs) = default;
    ~static_pooled_class(void) = default;

private:
    /** free one object from freelist
     *
     *  \return true if freelist is empty
     *
     * */
    static inline bool free_one_disposed_object(void) {
        void* chunk = disposed_objects.pop();
        if (chunk == nullptr)
            return true;
        object_pool.free(chunk);
        return false;
    }

    struct disposing_allocator {
        static void* allocate(std::size_t size) {
            free_disposed_objects();
            return object_pool.malloc(size);
        }
    };

    struct dispose_n_object_allocator {
        static void* allocate(std::size_t size) {
            for (unsigned int i = 0; i != recover_count; ++i) {
                bool freelist_empty = free_one_disposed_object();
                if (freelist_empty)
                    break;
            }

            for (;;) {
                void* ret = object_pool.malloc(size);

                if (ret)
                    return ret;
                if (free_one_disposed_object())
                    return nullptr; /* no object in freelist, we  */
            }
        }
    };

    typedef
        typename boost::mpl::if_c<(recover_count > 0), dispose_n_object_allocator, disposing_allocator>::type allocator;

public:
    static inline void* allocate(std::size_t size) {
#ifndef NOVA_MEMORY_DEBUGGING
        size = std::max(2 * sizeof(void*), size); /* size requirement for lockfree freelist */
        return allocator::allocate(size);
#else
        return malloc(size);
#endif
    }

    inline void* operator new(std::size_t size) { return allocate(size); }

    static inline void free_disposed_objects(void) {
        for (;;) {
            if (free_one_disposed_object())
                return;
        }
    }

    static inline void deallocate(void* p) {
#ifndef NOVA_MEMORY_DEBUGGING
        disposed_objects.push(p);
#else
        free(p);
#endif
    }

    inline void operator delete(void* p) { deallocate(p); }

    typedef static_pool<pool_size, pool_locking> object_pool_type;

    static object_pool_type object_pool;
    static freelist disposed_objects;
};

template <typename tag, std::size_t pool_size, bool pool_locking, unsigned int recover_count>
typename static_pooled_class<tag, pool_size, pool_locking, recover_count>::object_pool_type
    static_pooled_class<tag, pool_size, pool_locking, recover_count>::object_pool(true);

template <typename tag, std::size_t pool_size, bool pool_locking, unsigned int recover_count>
freelist static_pooled_class<tag, pool_size, pool_locking, recover_count>::disposed_objects;


} /* namespace nova */
