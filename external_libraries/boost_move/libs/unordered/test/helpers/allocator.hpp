
// Copyright 2006-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_UNORDERED_TEST_MALLOC_ALLOCATOR_HEADER)
#define BOOST_UNORDERED_TEST_MALLOC_ALLOCATOR_HEADER

#include <cstddef>
#include <cstdlib>
#include <boost/limits.hpp>
#include <new>

#if defined(BOOST_MSVC)
#pragma warning(push)
#pragma warning(disable:4100) // unreferenced formal parameter
#endif

namespace test
{
    template <class T>
    struct malloc_allocator
    {
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef T* pointer;
        typedef T const* const_pointer;
        typedef T& reference;
        typedef T const& const_reference;
        typedef T value_type;

        template <class U> struct rebind { typedef malloc_allocator<U> other; };

        malloc_allocator() {}
        template <class Y> malloc_allocator(malloc_allocator<Y> const&) {}
        malloc_allocator(malloc_allocator const&) {}

        pointer address(reference r) { return &r; }
        const_pointer address(const_reference r) { return &r; }

        pointer allocate(size_type n) {
            using namespace std;
            T* ptr = static_cast<T*>(malloc(n * sizeof(T)));
            if(!ptr) throw std::bad_alloc();
            return ptr;
        }

        pointer allocate(size_type n, void const* u) { return allocate(n); }

        void deallocate(pointer p, size_type) {
            using namespace std;
            free(p);
        }
        void construct(pointer p, T const& t) { new(p) T(t); }
        void destroy(pointer p) { p->~T(); }

        size_type max_size() const {
            return (std::numeric_limits<size_type>::max)();
        }

        bool operator==(malloc_allocator const& x) const { return true; }
        bool operator!=(malloc_allocator const& x) const { return false; }

#if BOOST_WORKAROUND(BOOST_MSVC, < 1300)
        template <class T> void deallocate(T* p, size_type) {
            using namespace std;
            free(p);
        }
        char* _Charalloc(size_type n) {
            using namespace std;
            T* ptr = static_cast<T*>(malloc(n * sizeof(char)));
            if(!ptr) throw std::bad_alloc();
            return (char*) ptr;
        }
#endif
    };
}

#if defined(BOOST_MSVC)
#pragma warning(pop)
#pragma warning(disable:4100) // unreferenced formal parameter
#endif

#endif
