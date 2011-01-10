
// Copyright (C) 2003-2004 Jeremy B. Maitin-Shepard.
// Copyright (C) 2005-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/unordered for documentation

#ifndef BOOST_UNORDERED_UNORDERED_SET_HPP_INCLUDED
#define BOOST_UNORDERED_UNORDERED_SET_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/move/move.hpp>
#include <boost/unordered/unordered_set_fwd.hpp>
#include <boost/functional/hash.hpp>
#include <boost/unordered/detail/hash_table.hpp>

#if defined(BOOST_MSVC)
#pragma warning(push)
#if BOOST_MSVC >= 1400
#pragma warning(disable:4396) //the inline specifier cannot be used when a
                              // friend declaration refers to a specialization
                              // of a function template
#endif
#endif

namespace boost
{
    template <class Value, class Hash, class Pred, class Alloc>
    class unordered_set
    {
        typedef boost::unordered_detail::hash_types_unique_keys<
            Value, Value, Hash, Pred, Alloc
        > implementation;

        BOOST_DEDUCED_TYPENAME implementation::hash_table base;

    public:

        // types

        typedef Value key_type;
        typedef Value value_type;
        typedef Hash hasher;
        typedef Pred key_equal;

        typedef Alloc allocator_type;
        typedef BOOST_DEDUCED_TYPENAME allocator_type::pointer pointer;
        typedef BOOST_DEDUCED_TYPENAME allocator_type::const_pointer const_pointer;
        typedef BOOST_DEDUCED_TYPENAME allocator_type::reference reference;
        typedef BOOST_DEDUCED_TYPENAME allocator_type::const_reference const_reference;

        typedef BOOST_DEDUCED_TYPENAME implementation::size_type size_type;
        typedef BOOST_DEDUCED_TYPENAME implementation::difference_type difference_type;

        typedef BOOST_DEDUCED_TYPENAME implementation::const_iterator iterator;
        typedef BOOST_DEDUCED_TYPENAME implementation::const_iterator const_iterator;
        typedef BOOST_DEDUCED_TYPENAME implementation::const_local_iterator local_iterator;
        typedef BOOST_DEDUCED_TYPENAME implementation::const_local_iterator const_local_iterator;

        // construct/destroy/copy

        explicit unordered_set(
                size_type n = boost::unordered_detail::default_initial_bucket_count,
                const hasher &hf = hasher(),
                const key_equal &eql = key_equal(),
                const allocator_type &a = allocator_type())
            : base(n, hf, eql, a)
        {
        }

        explicit unordered_set(allocator_type const& a)
            : base(boost::unordered_detail::default_initial_bucket_count,
                hasher(), key_equal(), a)
        {
        }

        unordered_set(unordered_set const& other, allocator_type const& a)
            : base(other.base, a)
        {
        }

        template <class InputIterator>
        unordered_set(InputIterator f, InputIterator l)
            : base(f, l, boost::unordered_detail::default_initial_bucket_count,
                hasher(), key_equal(), allocator_type())
        {
        }

        template <class InputIterator>
        unordered_set(InputIterator f, InputIterator l, size_type n,
                const hasher &hf = hasher(),
                const key_equal &eql = key_equal(),
                const allocator_type &a = allocator_type())
            : base(f, l, n, hf, eql, a)
        {
        }

        BOOST_ENABLE_MOVE_EMULATION(unordered_set)

        unordered_set(BOOST_RV_REF(unordered_set) other)
            : base(other.base, boost::unordered_detail::move_tag())
        {
        }

        unordered_set(BOOST_RV_REF(unordered_set) other, allocator_type const& a)
            : base(other.base, a, boost::unordered_detail::move_tag())
        {
        }

#if defined(BOOST_HAS_RVALUE_REFS)
        unordered_set& operator=(unordered_set&& x)
        {
            base.move(x.base);
            return *this;
        }
#elif !BOOST_WORKAROUND(__BORLANDC__, < 0x0593)
        unordered_set& operator=(unordered_set x)
        {
            base.move(x.base);
            return *this;
        }
#endif

#if !defined(BOOST_NO_INITIALIZER_LISTS)
        unordered_set(std::initializer_list<value_type> list,
                size_type n = boost::unordered_detail::default_initial_bucket_count,
                const hasher &hf = hasher(),
                const key_equal &eql = key_equal(),
                const allocator_type &a = allocator_type())
            : base(list.begin(), list.end(), n, hf, eql, a)
        {
        }

        unordered_set& operator=(std::initializer_list<value_type> list)
        {
            base.data_.clear();
            base.insert_range(list.begin(), list.end());
            return *this;
        }
#endif

    private:

        BOOST_DEDUCED_TYPENAME implementation::iterator_base const&
            get(const_iterator const& it)
        {
            return boost::unordered_detail::iterator_access::get(it);
        }

    public:

        allocator_type get_allocator() const
        {
            return base.get_allocator();
        }

        // size and capacity

        bool empty() const
        {
            return base.empty();
        }

        size_type size() const
        {
            return base.size();
        }

        size_type max_size() const
        {
            return base.max_size();
        }

        // iterators

        iterator begin()
        {
            return iterator(base.data_.begin());
        }

        const_iterator begin() const
        {
            return const_iterator(base.data_.begin());
        }

        iterator end()
        {
            return iterator(base.data_.end());
        }

        const_iterator end() const
        {
            return const_iterator(base.data_.end());
        }

        const_iterator cbegin() const
        {
            return const_iterator(base.data_.begin());
        }

        const_iterator cend() const
        {
            return const_iterator(base.data_.end());
        }

        // modifiers

#if defined(BOOST_HAS_RVALUE_REFS) && defined(BOOST_HAS_VARIADIC_TMPL)
        template <class... Args>
        std::pair<iterator, bool> emplace(Args&&... args)
        {
            return boost::unordered_detail::pair_cast<iterator, bool>(
                base.emplace(std::forward<Args>(args)...));
        }

        template <class... Args>
        iterator emplace_hint(const_iterator hint, Args&&... args)
        {
            return iterator(
                base.emplace_hint(get(hint), std::forward<Args>(args)...));
        }
#else

        std::pair<iterator, bool> emplace(value_type v = value_type())
        {
            return boost::unordered_detail::pair_cast<iterator, bool>(
                base.emplace(boost::move(v)));
        }

        iterator emplace_hint(const_iterator hint, value_type v = value_type())
        {
            return iterator(
                base.emplace_hint(get(hint), boost::move(v)));
        }

#define BOOST_UNORDERED_EMPLACE(z, n, _)                                        \
            template <                                                          \
                BOOST_UNORDERED_TEMPLATE_ARGS(z, n)                             \
            >                                                                   \
            std::pair<iterator, bool> emplace(                                  \
                BOOST_UNORDERED_FUNCTION_PARAMS(z, n)                           \
            )                                                                   \
            {                                                                   \
                return boost::unordered_detail::pair_cast<iterator, bool>(      \
                    base.emplace(                                               \
                        BOOST_UNORDERED_CALL_PARAMS(z, n)                       \
                    ));                                                         \
            }                                                                   \
                                                                                \
            template <                                                          \
                BOOST_UNORDERED_TEMPLATE_ARGS(z, n)                             \
            >                                                                   \
            iterator emplace_hint(const_iterator hint,                          \
                BOOST_UNORDERED_FUNCTION_PARAMS(z, n)                           \
            )                                                                   \
            {                                                                   \
                return iterator(base.emplace_hint(get(hint),                    \
                        BOOST_UNORDERED_CALL_PARAMS(z, n)                       \
                ));                                                             \
            }

        BOOST_PP_REPEAT_FROM_TO(1, BOOST_UNORDERED_EMPLACE_LIMIT,
            BOOST_UNORDERED_EMPLACE, _)

#undef BOOST_UNORDERED_EMPLACE

#endif

        std::pair<iterator, bool> insert(const value_type& obj)
        {
            return boost::unordered_detail::pair_cast<iterator, bool>(
                    base.emplace(obj));
        }

        iterator insert(const_iterator hint, const value_type& obj)
        {
            return iterator(base.emplace_hint(get(hint), obj));
        }

        template <class InputIterator>
            void insert(InputIterator first, InputIterator last)
        {
            base.insert_range(first, last);
        }

        iterator erase(const_iterator position)
        {
            return iterator(base.data_.erase(get(position)));
        }

        size_type erase(const key_type& k)
        {
            return base.erase_key(k);
        }

        iterator erase(const_iterator first, const_iterator last)
        {
            return iterator(base.data_.erase_range(get(first), get(last)));
        }

        void clear()
        {
            base.data_.clear();
        }

        void swap(unordered_set& other)
        {
            base.swap(other.base);
        }

        // observers

        hasher hash_function() const
        {
            return base.hash_function();
        }

        key_equal key_eq() const
        {
            return base.key_eq();
        }

        // lookup

        const_iterator find(const key_type& k) const
        {
            return const_iterator(base.find(k));
        }

        size_type count(const key_type& k) const
        {
            return base.count(k);
        }

        std::pair<const_iterator, const_iterator>
            equal_range(const key_type& k) const
        {
            return boost::unordered_detail::pair_cast<const_iterator, const_iterator>(
                    base.equal_range(k));
        }

        // bucket interface

        size_type bucket_count() const
        {
            return base.bucket_count();
        }

        size_type max_bucket_count() const
        {
            return base.max_bucket_count();
        }

        size_type bucket_size(size_type n) const
        {
            return base.data_.bucket_size(n);
        }

        size_type bucket(const key_type& k) const
        {
            return base.bucket(k);
        }

        local_iterator begin(size_type n)
        {
            return local_iterator(base.data_.begin(n));
        }

        const_local_iterator begin(size_type n) const
        {
            return const_local_iterator(base.data_.begin(n));
        }

        local_iterator end(size_type n)
        {
            return local_iterator(base.data_.end(n));
        }

        const_local_iterator end(size_type n) const
        {
            return const_local_iterator(base.data_.end(n));
        }

        const_local_iterator cbegin(size_type n) const
        {
            return const_local_iterator(base.data_.begin(n));
        }

        const_local_iterator cend(size_type n) const
        {
            return const_local_iterator(base.data_.end(n));
        }

        // hash policy

        float load_factor() const
        {
            return base.load_factor();
        }

        float max_load_factor() const
        {
            return base.max_load_factor();
        }

        void max_load_factor(float m)
        {
            base.max_load_factor(m);
        }

        void rehash(size_type n)
        {
            base.rehash(n);
        }

#if BOOST_WORKAROUND(BOOST_MSVC, < 1300)
        friend bool operator==(unordered_set const&, unordered_set const&);
        friend bool operator!=(unordered_set const&, unordered_set const&);
#else
        friend bool operator==<Value, Hash, Pred, Alloc>(unordered_set const&, unordered_set const&);
        friend bool operator!=<Value, Hash, Pred, Alloc>(unordered_set const&, unordered_set const&);
#endif
    }; // class template unordered_set

    template <class T, class H, class P, class A>
    inline bool operator==(unordered_set<T, H, P, A> const& m1,
        unordered_set<T, H, P, A> const& m2)
    {
        return boost::unordered_detail::equals(m1.base, m2.base);
    }

    template <class T, class H, class P, class A>
    inline bool operator!=(unordered_set<T, H, P, A> const& m1,
        unordered_set<T, H, P, A> const& m2)
    {
        return !boost::unordered_detail::equals(m1.base, m2.base);
    }

    template <class T, class H, class P, class A>
    inline void swap(unordered_set<T, H, P, A> &m1,
            unordered_set<T, H, P, A> &m2)
    {
        m1.swap(m2);
    }

    template <class Value, class Hash, class Pred, class Alloc>
    class unordered_multiset
    {
        typedef boost::unordered_detail::hash_types_equivalent_keys<
            Value, Value, Hash, Pred, Alloc
        > implementation;

        BOOST_DEDUCED_TYPENAME implementation::hash_table base;

    public:

        //types

        typedef Value key_type;
        typedef Value value_type;
        typedef Hash hasher;
        typedef Pred key_equal;

        typedef Alloc allocator_type;
        typedef BOOST_DEDUCED_TYPENAME allocator_type::pointer pointer;
        typedef BOOST_DEDUCED_TYPENAME allocator_type::const_pointer const_pointer;
        typedef BOOST_DEDUCED_TYPENAME allocator_type::reference reference;
        typedef BOOST_DEDUCED_TYPENAME allocator_type::const_reference const_reference;

        typedef BOOST_DEDUCED_TYPENAME implementation::size_type size_type;
        typedef BOOST_DEDUCED_TYPENAME implementation::difference_type difference_type;

        typedef BOOST_DEDUCED_TYPENAME implementation::const_iterator iterator;
        typedef BOOST_DEDUCED_TYPENAME implementation::const_iterator const_iterator;
        typedef BOOST_DEDUCED_TYPENAME implementation::const_local_iterator local_iterator;
        typedef BOOST_DEDUCED_TYPENAME implementation::const_local_iterator const_local_iterator;

        // construct/destroy/copy

        explicit unordered_multiset(
                size_type n = boost::unordered_detail::default_initial_bucket_count,
                const hasher &hf = hasher(),
                const key_equal &eql = key_equal(),
                const allocator_type &a = allocator_type())
          : base(n, hf, eql, a)
        {
        }

        explicit unordered_multiset(allocator_type const& a)
            : base(boost::unordered_detail::default_initial_bucket_count,
                hasher(), key_equal(), a)
        {
        }

        unordered_multiset(unordered_multiset const& other, allocator_type const& a)
            : base(other.base, a)
        {
        }

        template <class InputIterator>
        unordered_multiset(InputIterator f, InputIterator l)
            : base(f, l, boost::unordered_detail::default_initial_bucket_count,
                hasher(), key_equal(), allocator_type())
        {
        }

        template <class InputIterator>
        unordered_multiset(InputIterator f, InputIterator l, size_type n,
                const hasher &hf = hasher(),
                const key_equal &eql = key_equal(),
                const allocator_type &a = allocator_type())
          : base(f, l, n, hf, eql, a)
        {
        }

        BOOST_ENABLE_MOVE_EMULATION(unordered_multiset)

        unordered_multiset(BOOST_RV_REF(unordered_multiset) other)
            : base(other.base, boost::unordered_detail::move_tag())
        {
        }

        unordered_multiset(BOOST_RV_REF(unordered_multiset) other, allocator_type const& a)
            : base(other.base, a, boost::unordered_detail::move_tag())
        {
        }

#if defined(BOOST_HAS_RVALUE_REFS)
        unordered_multiset& operator=(unordered_multiset&& x)
        {
            base.move(x.base);
            return *this;
        }
#elif !BOOST_WORKAROUND(__BORLANDC__, < 0x0593)
        unordered_multiset& operator=(unordered_multiset x)
        {
            base.move(x.base);
            return *this;
        }
#endif

#if !defined(BOOST_NO_INITIALIZER_LISTS)
        unordered_multiset(std::initializer_list<value_type> list,
                size_type n = boost::unordered_detail::default_initial_bucket_count,
                const hasher &hf = hasher(),
                const key_equal &eql = key_equal(),
                const allocator_type &a = allocator_type())
            : base(list.begin(), list.end(), n, hf, eql, a)
        {
        }

        unordered_multiset& operator=(std::initializer_list<value_type> list)
        {
            base.data_.clear();
            base.insert_range(list.begin(), list.end());
            return *this;
        }
#endif

    private:

        BOOST_DEDUCED_TYPENAME implementation::iterator_base const&
            get(const_iterator const& it)
        {
            return boost::unordered_detail::iterator_access::get(it);
        }

    public:

        allocator_type get_allocator() const
        {
            return base.get_allocator();
        }

        // size and capacity

        bool empty() const
        {
            return base.empty();
        }

        size_type size() const
        {
            return base.size();
        }

        size_type max_size() const
        {
            return base.max_size();
        }

        // iterators

        iterator begin()
        {
            return iterator(base.data_.begin());
        }

        const_iterator begin() const
        {
            return const_iterator(base.data_.begin());
        }

        iterator end()
        {
            return iterator(base.data_.end());
        }

        const_iterator end() const
        {
            return const_iterator(base.data_.end());
        }

        const_iterator cbegin() const
        {
            return const_iterator(base.data_.begin());
        }

        const_iterator cend() const
        {
            return const_iterator(base.data_.end());
        }

        // modifiers

#if defined(BOOST_HAS_RVALUE_REFS) && defined(BOOST_HAS_VARIADIC_TMPL)
        template <class... Args>
        iterator emplace(Args&&... args)
        {
            return iterator(base.emplace(std::forward<Args>(args)...));
        }

        template <class... Args>
        iterator emplace_hint(const_iterator hint, Args&&... args)
        {
            return iterator(base.emplace_hint(get(hint), std::forward<Args>(args)...));
        }
#else

        iterator emplace(value_type v = value_type())
        {
            return iterator(base.emplace(boost::move(v)));
        }

        iterator emplace_hint(const_iterator hint, value_type v = value_type())
        {
            return iterator(base.emplace_hint(get(hint), boost::move(v)));
        }

#define BOOST_UNORDERED_EMPLACE(z, n, _)                                        \
            template <                                                          \
                BOOST_UNORDERED_TEMPLATE_ARGS(z, n)                             \
            >                                                                   \
            iterator emplace(                                                   \
                BOOST_UNORDERED_FUNCTION_PARAMS(z, n)                           \
            )                                                                   \
            {                                                                   \
                return iterator(                                                \
                    base.emplace(                                               \
                        BOOST_UNORDERED_CALL_PARAMS(z, n)                       \
                    ));                                                         \
            }                                                                   \
                                                                                \
            template <                                                          \
                BOOST_UNORDERED_TEMPLATE_ARGS(z, n)                             \
            >                                                                   \
            iterator emplace_hint(const_iterator hint,                          \
                BOOST_UNORDERED_FUNCTION_PARAMS(z, n)                           \
            )                                                                   \
            {                                                                   \
                return iterator(base.emplace_hint(get(hint),                    \
                        BOOST_UNORDERED_CALL_PARAMS(z, n)                       \
                ));                                                             \
            }

        BOOST_PP_REPEAT_FROM_TO(1, BOOST_UNORDERED_EMPLACE_LIMIT,
            BOOST_UNORDERED_EMPLACE, _)

#undef BOOST_UNORDERED_EMPLACE

#endif

        iterator insert(const value_type& obj)
        {
            return iterator(base.emplace(obj));
        }

        iterator insert(const_iterator hint, const value_type& obj)
        {
            return iterator(base.emplace_hint(get(hint), obj));
        }

        template <class InputIterator>
            void insert(InputIterator first, InputIterator last)
        {
            base.insert_range(first, last);
        }

        iterator erase(const_iterator position)
        {
            return iterator(base.data_.erase(get(position)));
        }

        size_type erase(const key_type& k)
        {
            return base.erase_key(k);
        }

        iterator erase(const_iterator first, const_iterator last)
        {
            return iterator(base.data_.erase_range(get(first), get(last)));
        }

        void clear()
        {
            base.data_.clear();
        }

        void swap(unordered_multiset& other)
        {
            base.swap(other.base);
        }

        // observers

        hasher hash_function() const
        {
            return base.hash_function();
        }

        key_equal key_eq() const
        {
            return base.key_eq();
        }

        // lookup

        const_iterator find(const key_type& k) const
        {
            return const_iterator(base.find(k));
        }

        size_type count(const key_type& k) const
        {
            return base.count(k);
        }

        std::pair<const_iterator, const_iterator>
            equal_range(const key_type& k) const
        {
            return boost::unordered_detail::pair_cast<const_iterator, const_iterator>(
                    base.equal_range(k));
        }

        // bucket interface

        size_type bucket_count() const
        {
            return base.bucket_count();
        }

        size_type max_bucket_count() const
        {
            return base.max_bucket_count();
        }

        size_type bucket_size(size_type n) const
        {
            return base.data_.bucket_size(n);
        }

        size_type bucket(const key_type& k) const
        {
            return base.bucket(k);
        }

        local_iterator begin(size_type n)
        {
            return local_iterator(base.data_.begin(n));
        }

        const_local_iterator begin(size_type n) const
        {
            return const_local_iterator(base.data_.begin(n));
        }

        local_iterator end(size_type n)
        {
            return local_iterator(base.data_.end(n));
        }

        const_local_iterator end(size_type n) const
        {
            return const_local_iterator(base.data_.end(n));
        }

        const_local_iterator cbegin(size_type n) const
        {
            return const_local_iterator(base.data_.begin(n));
        }

        const_local_iterator cend(size_type n) const
        {
            return const_local_iterator(base.data_.end(n));
        }

        // hash policy

        float load_factor() const
        {
            return base.load_factor();
        }

        float max_load_factor() const
        {
            return base.max_load_factor();
        }

        void max_load_factor(float m)
        {
            base.max_load_factor(m);
        }

        void rehash(size_type n)
        {
            base.rehash(n);
        }

#if BOOST_WORKAROUND(BOOST_MSVC, < 1300)
        friend bool operator==(unordered_multiset const&, unordered_multiset const&);
        friend bool operator!=(unordered_multiset const&, unordered_multiset const&);
#else
        friend bool operator==<Value, Hash, Pred, Alloc>(unordered_multiset const&, unordered_multiset const&);
        friend bool operator!=<Value, Hash, Pred, Alloc>(unordered_multiset const&, unordered_multiset const&);
#endif
    }; // class template unordered_multiset

    template <class T, class H, class P, class A>
    inline bool operator==(unordered_multiset<T, H, P, A> const& m1,
        unordered_multiset<T, H, P, A> const& m2)
    {
        return boost::unordered_detail::equals(m1.base, m2.base);
    }

    template <class T, class H, class P, class A>
    inline bool operator!=(unordered_multiset<T, H, P, A> const& m1,
        unordered_multiset<T, H, P, A> const& m2)
    {
        return !boost::unordered_detail::equals(m1.base, m2.base);
    }

    template <class T, class H, class P, class A>
    inline void swap(unordered_multiset<T, H, P, A> &m1,
            unordered_multiset<T, H, P, A> &m2)
    {
        m1.swap(m2);
    }

} // namespace boost

#if defined(BOOST_MSVC)
#pragma warning(pop)
#endif

#endif // BOOST_UNORDERED_UNORDERED_SET_HPP_INCLUDED
