//  functions for aligned memory allocation
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

#include <cstdlib>
#include <cstring>

#include <new> // for std::bad_alloc
#include <utility> // for std::forward

#include <boost/align/aligned_alloc.hpp>

#include "function_attributes.h"

namespace nova {

const int malloc_memory_alignment = 64;

inline void* MALLOC ASSUME_ALIGNED(64) malloc_aligned(std::size_t bytes) {
    return boost::alignment::aligned_alloc(malloc_memory_alignment, bytes);
}

inline void free_aligned(void* ptr) { return boost::alignment::aligned_free(ptr); }

inline void* MALLOC ASSUME_ALIGNED(64) calloc_aligned(std::size_t bytes) {
    void* ret = malloc_aligned(bytes);
    if (ret)
        std::memset(ret, 0, bytes);
    return ret;
}

template <typename T> T* MALLOC ASSUME_ALIGNED(64) malloc_aligned(std::size_t n) {
    return static_cast<T*>(malloc_aligned(n * sizeof(T)));
}

template <typename T> T* MALLOC ASSUME_ALIGNED(64) calloc_aligned(std::size_t n) {
    return static_cast<T*>(calloc_aligned(n * sizeof(T)));
}


/** smart-pointer, freeing the managed pointer via free_aligned */
template <class T, bool managed = true> class aligned_storage_ptr {
public:
    explicit aligned_storage_ptr(T* p = 0): ptr(p) {}

    explicit aligned_storage_ptr(size_t count): ptr(malloc_aligned<T>(count)) {}

    ~aligned_storage_ptr(void) {
        if (managed && ptr)
            free_aligned(ptr);
    }

    void reset(T* p = 0) {
        if (managed && ptr)
            free_aligned(ptr);
        ptr = p;
    }

    T& operator*() const { return *ptr; }

    T* operator->() const { return ptr; }

    T* get() const { return ptr; }

    aligned_storage_ptr& operator=(T* p) {
        reset(p);
        return *this;
    }

    operator bool() const { return bool(ptr); }

    void swap(aligned_storage_ptr& b) {
        T* p = ptr;
        ptr = b.ptr;
        b.ptr = p;
    }

private:
    T* ptr;
};

} /* namespace nova */
