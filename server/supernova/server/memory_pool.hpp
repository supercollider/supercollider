//  real-time memory pool
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

#include "../utilities/simple_pool.hpp"

#include <stdexcept>

namespace nova {

extern simple_pool<false> rt_pool; /* real-time memory pool */

/** stl-compliant allocator class, using the rt_pool */
template <class T> class rt_pool_allocator {
public:
    using value_type = T;

    rt_pool_allocator() noexcept = default;
    rt_pool_allocator(const rt_pool_allocator&) noexcept = default;
    ~rt_pool_allocator() noexcept = default;

    template <class U> rt_pool_allocator(const rt_pool_allocator<U>&) noexcept {}

    T* allocate(size_t size) {
        T* ret = static_cast<T*>(rt_pool.malloc(size * sizeof(T)));
        if (ret == nullptr)
            throw std::bad_alloc();

        return ret;
    }

    T* reallocate(T* p, size_t size) {
        T* ret = static_cast<T*>(rt_pool.realloc(p, size));
        if (ret == nullptr)
            throw std::bad_alloc();

        return ret;
    }

    void deallocate(T* p, size_t n) { rt_pool.free(p); }

    size_t max_size() const noexcept { return rt_pool.get_max_size(); }
};

} /* namespace nova */
