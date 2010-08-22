
// Copyright 2006-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Define some minimal classes which provide the bare minimum concepts to
// test that the containers don't rely on something that they shouldn't.
// They are not intended to be good examples of how to implement the concepts.

#if !defined(BOOST_UNORDERED_OBJECTS_MINIMAL_HEADER)
#define BOOST_UNORDERED_OBJECTS_MINIMAL_HEADER

#include <cstddef>

#if defined(BOOST_MSVC)
#pragma warning(push)
#pragma warning(disable:4100) // unreferenced formal parameter
#endif

namespace test
{
namespace minimal
{
    class copy_constructible;
    class copy_constructible_equality_comparable;
    class default_copy_constructible;
    class assignable;

    template <class T> class hash;
    template <class T> class equal_to;
    template <class T> class ptr;
    template <class T> class const_ptr;
    template <class T> class allocator;

    class copy_constructible
    {
    public:
        static copy_constructible create() { return copy_constructible(); }
        copy_constructible(copy_constructible const&) {}
        ~copy_constructible() {}
    private:
        copy_constructible& operator=(copy_constructible const&);
        copy_constructible() {}
    };

    class copy_constructible_equality_comparable
    {
    public:
        static copy_constructible_equality_comparable create() { return copy_constructible_equality_comparable(); }
        copy_constructible_equality_comparable(copy_constructible_equality_comparable const&) {}
        ~copy_constructible_equality_comparable() {}
    private:
        copy_constructible_equality_comparable& operator=(copy_constructible_equality_comparable const&);
        copy_constructible_equality_comparable() {}
    };

    bool operator==(copy_constructible_equality_comparable, copy_constructible_equality_comparable) {
        return true;
    }

    bool operator!=(copy_constructible_equality_comparable, copy_constructible_equality_comparable) {
        return false;
    }

    class default_copy_constructible
    {
    public:
        static default_copy_constructible create() { return default_copy_constructible(); }
        default_copy_constructible() {}
        default_copy_constructible(default_copy_constructible const&) {}
        ~default_copy_constructible() {}
    private:
        default_copy_constructible& operator=(default_copy_constructible const&);
    };

    class assignable
    {
    public:
        static assignable create() { return assignable(); }
        assignable(assignable const&) {}
        assignable& operator=(assignable const&) { return *this; }
        ~assignable() {}
    private:
        assignable() {}
    };

    template <class T>
    class hash
    {
    public:
        static hash create() { return hash<T>(); }
        hash() {}
        hash(hash const&) {}
        hash& operator=(hash const&) { return *this; }
        ~hash() {}

        std::size_t operator()(T const&) const { return 0; }
    };

    template <class T>
    class equal_to
    {
    public:
        static equal_to create() { return equal_to<T>(); }
        equal_to() {}
        equal_to(equal_to const&) {}
        equal_to& operator=(equal_to const&) { return *this; }
        ~equal_to() {}

        bool operator()(T const&, T const&) const { return true; }
    };

    template <class T> class ptr;
    template <class T> class const_ptr;

    template <class T>
    class ptr
    {
        friend class allocator<T>;
        friend class const_ptr<T>;

        T* ptr_;

        ptr(T* x) : ptr_(x) {}
    public:
        ptr() : ptr_(0) {}

        T& operator*() const { return *ptr_; }
        T* operator->() const { return ptr_; }
        ptr& operator++() { ++ptr_; return *this; }
        ptr operator++(int) { ptr tmp(*this); ++ptr_; return tmp; }
        ptr operator+(std::ptrdiff_t s) const { return ptr<T>(ptr_ + s); }
        friend ptr operator+(std::ptrdiff_t s, ptr p) { return ptr<T>(s + p.ptr_); }
        T& operator[](std::ptrdiff_t s) const { return ptr_[s]; }
        bool operator!() const { return !ptr_; }
        
        // I'm not using the safe bool idiom because the containers should be
        // able to cope with bool conversions.
        operator bool() const { return !!ptr_; }

        bool operator==(ptr const& x) const { return ptr_ == x.ptr_; }
        bool operator!=(ptr const& x) const { return ptr_ != x.ptr_; }
        bool operator<(ptr const& x) const { return ptr_ < x.ptr_; }
        bool operator>(ptr const& x) const { return ptr_ > x.ptr_; }
        bool operator<=(ptr const& x) const { return ptr_ <= x.ptr_; }
        bool operator>=(ptr const& x) const { return ptr_ >= x.ptr_; }

        bool operator==(const_ptr<T> const& x) const { return ptr_ == x.ptr_; }
        bool operator!=(const_ptr<T> const& x) const { return ptr_ != x.ptr_; }
        bool operator<(const_ptr<T> const& x) const { return ptr_ < x.ptr_; }
        bool operator>(const_ptr<T> const& x) const { return ptr_ > x.ptr_; }
        bool operator<=(const_ptr<T> const& x) const { return ptr_ <= x.ptr_; }
        bool operator>=(const_ptr<T> const& x) const { return ptr_ >= x.ptr_; }
    };

    template <class T>
    class const_ptr
    {
        friend class allocator<T>;

        T const* ptr_;

        const_ptr(T const* ptr) : ptr_(ptr) {}
    public:
        const_ptr() : ptr_(0) {}
        const_ptr(ptr<T> const& x) : ptr_(x.ptr_) {}

        T const& operator*() const { return *ptr_; }
        T const* operator->() const { return ptr_; }
        const_ptr& operator++() { ++ptr_; return *this; }
        const_ptr operator++(int) { const_ptr tmp(*this); ++ptr_; return tmp; }
        const_ptr operator+(std::ptrdiff_t s) const { return const_ptr(ptr_ + s); }
        friend const_ptr operator+(std::ptrdiff_t s, const_ptr p) { return ptr<T>(s + p.ptr_); }
        T const& operator[](int s) const { return ptr_[s]; }
        bool operator!() const { return !ptr_; }
        operator bool() const { return !!ptr_; }

        bool operator==(ptr<T> const& x) const { return ptr_ == x.ptr_; }
        bool operator!=(ptr<T> const& x) const { return ptr_ != x.ptr_; }
        bool operator<(ptr<T> const& x) const { return ptr_ < x.ptr_; }
        bool operator>(ptr<T> const& x) const { return ptr_ > x.ptr_; }
        bool operator<=(ptr<T> const& x) const { return ptr_ <= x.ptr_; }
        bool operator>=(ptr<T> const& x) const { return ptr_ >= x.ptr_; }

        bool operator==(const_ptr const& x) const { return ptr_ == x.ptr_; }
        bool operator!=(const_ptr const& x) const { return ptr_ != x.ptr_; }
        bool operator<(const_ptr const& x) const { return ptr_ < x.ptr_; }
        bool operator>(const_ptr const& x) const { return ptr_ > x.ptr_; }
        bool operator<=(const_ptr const& x) const { return ptr_ <= x.ptr_; }
        bool operator>=(const_ptr const& x) const { return ptr_ >= x.ptr_; }
    };

    template <class T>
    class allocator
    {
    public:
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef ptr<T> pointer;
        typedef const_ptr<T> const_pointer;
        typedef T& reference;
        typedef T const& const_reference;
        typedef T value_type;

        template <class U> struct rebind { typedef allocator<U> other; };

        allocator() {}
        template <class Y> allocator(allocator<Y> const&) {}
        allocator(allocator const&) {}
        ~allocator() {}

        pointer address(reference r) { return pointer(&r); }
        const_pointer address(const_reference r) { return const_pointer(&r); }

        pointer allocate(size_type n) {
            return pointer(static_cast<T*>(::operator new(n * sizeof(T))));
        }

        template <class Y>
        pointer allocate(size_type n, const_ptr<Y> u)
        {
            return pointer(static_cast<T*>(::operator new(n * sizeof(T))));
        }

        void deallocate(pointer p, size_type)
        {
            ::operator delete((void*) p.ptr_);
        }

        void construct(pointer p, T const& t) { new((void*)p.ptr_) T(t); }

#if defined(BOOST_HAS_RVALUE_REFS) && defined(BOOST_HAS_VARIADIC_TMPL)
        template<class... Args> void construct(pointer p, Args&&... args) {
            new((void*)p.ptr_) T(std::forward<Args>(args)...);
        }
#endif

        void destroy(pointer p) { ((T*)p.ptr_)->~T(); }

        size_type max_size() const { return 1000; }

#if defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP) || \
        BOOST_WORKAROUND(BOOST_MSVC, <= 1300)
    public: allocator& operator=(allocator const&) { return *this;}
#else
    private: allocator& operator=(allocator const&);
#endif
    };

    template <class T>
    inline bool operator==(allocator<T> const&, allocator<T> const&)
    {
        return true;
    }

    template <class T>
    inline bool operator!=(allocator<T> const&, allocator<T> const&)
    {
        return false;
    }

    template <class T>
    void swap(allocator<T>&, allocator<T>&)
    {
    }
}
}

#if defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)
namespace boost {
#else
namespace test {
namespace minimal {
#endif
    std::size_t hash_value(test::minimal::copy_constructible_equality_comparable) {
        return 1;
    }
#if !defined(BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)
}}
#else
}
#endif


#if defined(BOOST_MSVC)
#pragma warning(pop)
#endif

#endif
