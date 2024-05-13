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

namespace nova {

extern simple_pool<false> rt_pool; /* real-time memory pool */

/** stl-compliant allocator class, using the rt_pool */
template <class T> class rt_pool_allocator {
public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    template <class U> struct rebind { typedef rt_pool_allocator<U> other; };

    rt_pool_allocator() throw() = default;
    rt_pool_allocator(const rt_pool_allocator&) throw() = default;
    ~rt_pool_allocator() throw() = default;

    template <class U> rt_pool_allocator(const rt_pool_allocator<U>&) throw() {}


    pointer address(reference x) const { return &x; }

    const_pointer address(const_reference x) const { return &x; }

    pointer allocate(size_type size, const void* hint = nullptr) {
        pointer ret = static_cast<pointer>(rt_pool.malloc(size * sizeof(T)));
        if (ret == nullptr)
            throw std::bad_alloc();

        return ret;
    }

    pointer reallocate(pointer p, size_type size, void* hint = nullptr) {
        return static_cast<pointer>(rt_pool.realloc(p, size));
    }

    void deallocate(pointer p, size_type n) { rt_pool.free(p); }

    size_type max_size() const throw() { return rt_pool.get_max_size(); }

    void construct(pointer p, const T& val = T()) { new (p) T(val); }

    template <class U, class... Args> void construct(U* p, Args&&... args) {
        ::new ((void*)p) U(std::forward<Args>(args)...);
    }

    void destroy(pointer p) { p->~T(); }

    bool operator==(rt_pool_allocator const& rhs) { return true; }

    template <typename AllocatorType> bool operator==(AllocatorType const& rhs) { return false; }
};

} /* namespace nova */
