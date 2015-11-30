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

#ifndef UTILITIES_MALLOC_ALIGNED_HPP
#define UTILITIES_MALLOC_ALIGNED_HPP

#include <cstdlib>
#include <cstring>

#include <new>     // for std::bad_alloc
#include <utility> // for std::forward

#ifdef __SSE2__
#include <xmmintrin.h>
#elif defined(HAVE_TBB)
#include <tbb/cache_aligned_allocator.h>
#endif /* HAVE_TBB */

#ifdef _MSC_VER
#include <malloc.h>
#endif

#include "function_attributes.h"

namespace nova {

#if _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
/* we have posix_memalign */

/* memory alignment constraints:
 *
 * - 16 byte for SSE operations
 * - the cache lines size of modern x86 cpus is 64 bytes (pentium-m, pentium 4, core, k8)
 */
const int malloc_memory_alignment = 64;

inline void* MALLOC ASSUME_ALIGNED(64) malloc_aligned(std::size_t nbytes)
{
    void * ret;
    int status = posix_memalign(&ret, malloc_memory_alignment, nbytes);
    if (!status)
        return ret;
    else
        return 0;
}

inline void free_aligned(void *ptr)
{
    free(ptr);
}

#elif defined(__SSE2__)

const int malloc_memory_alignment = 64;

inline void* MALLOC ASSUME_ALIGNED(64) malloc_aligned(std::size_t nbytes)
{
    return _mm_malloc(nbytes, malloc_memory_alignment);
}

inline void free_aligned(void *ptr)
{
    _mm_free(ptr);
}

#elif defined(__APPLE__)

const int malloc_memory_alignment = 64;

/* apple's malloc implementation returns 16-byte aligned chunks */
inline void* MALLOC malloc_aligned(std::size_t nbytes)
{
    return malloc(nbytes);
}

inline void free_aligned(void *ptr)
{
    free(ptr);
}


#elif defined(_MSC_VER)

const int malloc_memory_alignment = 64;

inline void* MALLOC ASSUME_ALIGNED(64) malloc_aligned(std::size_t nbytes)
{
    return _aligned_malloc(nbytes, malloc_memory_alignment);
}

inline void free_aligned(void *ptr)
{
    _aligned_free(ptr);
}

#elif defined(HAVE_TBB)

inline void* MALLOC malloc_aligned(std::size_t nbytes)
{
    tbb::cache_aligned_allocator<void*> ca_alloc;
    return static_cast<void*>(ca_alloc.allocate(nbytes));
}

inline void free_aligned(void *ptr)
{
    tbb::cache_aligned_allocator<void*> ca_alloc;
    ca_alloc.deallocate(static_cast<void**>(ptr), 0);
}

#else

/* on other systems, we use the aligned memory allocation taken
 * from thomas grill's implementation for pd */
#define VECTORALIGNMENT 128
inline void* MALLOC ASSUME_ALIGNED(64) malloc_aligned(std::size_t nbytes)
{
    void* vec = malloc(nbytes+ (VECTORALIGNMENT/8-1) + sizeof(void *));

    if (vec != nullptr) {
        /* get alignment of first possible signal vector byte */
        long alignment = ((long)vec+sizeof(void *))&(VECTORALIGNMENT/8-1);
        /* calculate aligned pointer */
        void *ret = (unsigned char *)vec+sizeof(void *)+(alignment == 0?0:VECTORALIGNMENT/8-alignment);
        /* save original memory location */
        *(void **)((unsigned char *)ret-sizeof(void *)) = vec;
        return ret;
    } else
        return 0;
}

inline void free_aligned(void *ptr)
{
    if (ptr == nullptr)
        return;

    /* get original memory location */
    void *ori = *(void **)((unsigned char *)ptr-sizeof(void *));
    free(ori);
}

#undef VECTORALIGNMENT

#endif

inline void * calloc_aligned(std::size_t nbytes)
{
    void * ret = malloc_aligned(nbytes);
    if (ret)
        std::memset(ret, 0, nbytes);
    return ret;
}

template <typename T>
T* malloc_aligned(std::size_t n)
{
    return static_cast<T*>(malloc_aligned(n * sizeof(T)));
}

template <typename T>
T* calloc_aligned(std::size_t n)
{
    return static_cast<T*>(calloc_aligned(n * sizeof(T)));
}


/** aligned allocator. uses malloc_aligned and free_aligned internally
 *  */
template <class T>
class aligned_allocator
{
public:
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T         value_type;

    template <class U> struct rebind
    {
        typedef aligned_allocator<U> other;
    };

    aligned_allocator(void)
    {}

    template <class U>
    aligned_allocator(aligned_allocator<U> const & rhs)
    {}

    pointer address(reference x) const
    {
        return &x;
    }

    const_pointer address(const_reference x) const
    {
        return &x;
    }

    pointer allocate(size_type n, const void* hint = nullptr)
    {
        pointer ret = malloc_aligned<T>(n);
        if (ret == nullptr)
            throw std::bad_alloc();
        return ret;
    }

    void deallocate(pointer p, size_type n)
    {
        return free_aligned(p);
    }

    size_type max_size() const throw()
    {
        return size_type(-1) / sizeof(T);
    }

    void construct(pointer p, const T& val = T())
    {
        ::new(p) T(val);
    }

#if (__cplusplus >= 201103L)
    template< class U, class... Args >
    void construct(U * p, Args&& ... args)
    {
        ::new((void*)p) U(std::forward<Args>(args)...);
    }
#endif

    void destroy(pointer p)
    {
        p->~T();
    }
};


template<typename T, typename U>
bool operator==( aligned_allocator<T> const& left, aligned_allocator<U> const& right )
{
    return !(left != right);
}

template<typename T, typename U>
bool operator!=( aligned_allocator<T> const& left, aligned_allocator<U> const& right )
{
    return true;
}


/** smart-pointer, freeing the managed pointer via free_aligned */
template<class T, bool managed = true>
class aligned_storage_ptr
{
public:
    explicit aligned_storage_ptr(T * p = 0):
        ptr(p)
    {}

    explicit aligned_storage_ptr(size_t count):
        ptr(malloc_aligned<T>(count))
    {}

    ~aligned_storage_ptr(void)
    {
        if (managed && ptr)
            free_aligned(ptr);
    }

    void reset(T * p = 0)
    {
        if (managed && ptr)
            free_aligned(ptr);
        ptr = p;
    }

    T & operator*() const
    {
        return *ptr;
    }

    T * operator->() const
    {
        return ptr;
    }

    T * get() const
    {
        return ptr;
    }

    aligned_storage_ptr & operator=(T * p)
    {
        reset(p);
        return *this;
    }

    operator bool() const
    {
        return bool(ptr);
    }

    void swap(aligned_storage_ptr & b)
    {
        T * p = ptr;
        ptr = b.ptr;
        b.ptr = p;
    }

private:
    T * ptr;
};

} /* namespace nova */

#endif /* UTILITIES_MALLOC_ALIGNED_HPP */
