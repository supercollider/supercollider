
// Copyright (C) 2003-2004 Jeremy B. Maitin-Shepard.
// Copyright (C) 2005-2011 Daniel James.
// Copyright (C) 2022 Christian Mazakas
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/unordered for documentation

#ifndef BOOST_UNORDERED_UNORDERED_MAP_HPP_INCLUDED
#define BOOST_UNORDERED_UNORDERED_MAP_HPP_INCLUDED

#include <boost/config.hpp>
#if defined(BOOST_HAS_PRAGMA_ONCE)
#pragma once
#endif

#include <boost/unordered/detail/requires_cxx11.hpp>
#include <boost/core/explicit_operator_bool.hpp>
#include <boost/functional/hash.hpp>
#include <boost/move/move.hpp>
#include <boost/type_traits/is_constructible.hpp>
#include <boost/unordered/detail/map.hpp>
#include <boost/unordered/detail/type_traits.hpp>

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
#include <initializer_list>
#endif

#if defined(BOOST_MSVC)
#pragma warning(push)
// conditional expression is constant
#pragma warning(disable : 4127)
#if BOOST_MSVC >= 1400
// the inline specifier cannot be used when a friend declaration refers to a
// specialization of a function template
#pragma warning(disable : 4396)
#endif
#endif

namespace boost {
  namespace unordered {
    template <class K, class T, class H, class P, class A> class unordered_map
    {
#if defined(BOOST_UNORDERED_USE_MOVE)
      BOOST_COPYABLE_AND_MOVABLE(unordered_map)
#endif
      template <typename, typename, typename, typename, typename>
      friend class unordered_multimap;

    public:
      typedef K key_type;
      typedef T mapped_type;
      typedef std::pair<const K, T> value_type;
      typedef typename boost::type_identity<H>::type hasher;
      typedef typename boost::type_identity<P>::type key_equal;
      typedef typename boost::type_identity<A>::type allocator_type;

    private:
      typedef boost::unordered::detail::map<A, K, T, H, P> types;
      typedef typename types::value_allocator_traits value_allocator_traits;
      typedef typename types::table table;

    public:
      typedef typename value_allocator_traits::pointer pointer;
      typedef typename value_allocator_traits::const_pointer const_pointer;

      typedef value_type& reference;
      typedef value_type const& const_reference;

      typedef std::size_t size_type;
      typedef std::ptrdiff_t difference_type;

      typedef typename table::iterator iterator;
      typedef typename table::c_iterator const_iterator;
      typedef typename table::l_iterator local_iterator;
      typedef typename table::cl_iterator const_local_iterator;
      typedef typename types::node_type node_type;
      typedef typename types::insert_return_type insert_return_type;

    private:
      table table_;

    public:
      // constructors

      unordered_map();

      explicit unordered_map(size_type, const hasher& = hasher(),
        const key_equal& = key_equal(),
        const allocator_type& = allocator_type());

      template <class InputIt>
      unordered_map(InputIt, InputIt,
        size_type = boost::unordered::detail::default_bucket_count,
        const hasher& = hasher(), const key_equal& = key_equal(),
        const allocator_type& = allocator_type());

      unordered_map(unordered_map const&);

#if defined(BOOST_UNORDERED_USE_MOVE) ||                                        \
  !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
      unordered_map(BOOST_RV_REF(unordered_map) other)
        BOOST_NOEXCEPT_IF(table::nothrow_move_constructible)
          : table_(other.table_, boost::unordered::detail::move_tag())
      {
        // The move is done in table_
      }
#endif

      explicit unordered_map(allocator_type const&);

      unordered_map(unordered_map const&, allocator_type const&);

      unordered_map(BOOST_RV_REF(unordered_map), allocator_type const&);

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
      unordered_map(std::initializer_list<value_type>,
        size_type = boost::unordered::detail::default_bucket_count,
        const hasher& = hasher(), const key_equal& l = key_equal(),
        const allocator_type& = allocator_type());
#endif

      explicit unordered_map(size_type, const allocator_type&);

      explicit unordered_map(size_type, const hasher&, const allocator_type&);

      template <class InputIterator>
      unordered_map(InputIterator, InputIterator, const allocator_type&);

      template <class InputIt>
      unordered_map(InputIt, InputIt, size_type, const allocator_type&);

      template <class InputIt>
      unordered_map(
        InputIt, InputIt, size_type, const hasher&, const allocator_type&);

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
      unordered_map(std::initializer_list<value_type>, const allocator_type&);

      unordered_map(
        std::initializer_list<value_type>, size_type, const allocator_type&);

      unordered_map(std::initializer_list<value_type>, size_type, const hasher&,
        const allocator_type&);
#endif

      // Destructor

      ~unordered_map() BOOST_NOEXCEPT;

// Assign

#if defined(BOOST_UNORDERED_USE_MOVE)
      unordered_map& operator=(BOOST_COPY_ASSIGN_REF(unordered_map) x)
      {
        table_.assign(x.table_, boost::unordered::detail::true_type());
        return *this;
      }

      unordered_map& operator=(BOOST_RV_REF(unordered_map) x)
        BOOST_NOEXCEPT_IF(value_allocator_traits::is_always_equal::value&&
            boost::is_nothrow_move_assignable<H>::value&&
              boost::is_nothrow_move_assignable<P>::value)
      {
        table_.move_assign(x.table_, boost::unordered::detail::true_type());
        return *this;
      }
#else
      unordered_map& operator=(unordered_map const& x)
      {
        table_.assign(x.table_, boost::unordered::detail::true_type());
        return *this;
      }

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
      unordered_map& operator=(unordered_map&& x)
        BOOST_NOEXCEPT_IF(value_allocator_traits::is_always_equal::value&&
            boost::is_nothrow_move_assignable<H>::value&&
              boost::is_nothrow_move_assignable<P>::value)
      {
        table_.move_assign(x.table_, boost::unordered::detail::true_type());
        return *this;
      }
#endif
#endif

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
      unordered_map& operator=(std::initializer_list<value_type>);
#endif

      allocator_type get_allocator() const BOOST_NOEXCEPT
      {
        return table_.node_alloc();
      }

//       // iterators

      iterator begin() BOOST_NOEXCEPT { return table_.begin(); }

      const_iterator begin() const BOOST_NOEXCEPT
      {
        return const_iterator(table_.begin());
      }

      iterator end() BOOST_NOEXCEPT { return iterator(); }

      const_iterator end() const BOOST_NOEXCEPT { return const_iterator(); }

      const_iterator cbegin() const BOOST_NOEXCEPT
      {
        return const_iterator(table_.begin());
      }

      const_iterator cend() const BOOST_NOEXCEPT { return const_iterator(); }

      // size and capacity

      BOOST_ATTRIBUTE_NODISCARD bool empty() const BOOST_NOEXCEPT
      {
        return table_.size_ == 0;
      }

      size_type size() const BOOST_NOEXCEPT { return table_.size_; }

      size_type max_size() const BOOST_NOEXCEPT;

// emplace

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

      template <class... Args>
      std::pair<iterator, bool> emplace(BOOST_FWD_REF(Args)... args)
      {
        return table_.emplace_unique(
          table::extractor::extract(boost::forward<Args>(args)...),
          boost::forward<Args>(args)...);
      }

#else

#if !BOOST_UNORDERED_SUN_WORKAROUNDS1

      // 0 argument emplace requires special treatment in case
      // the container is instantiated with a value type that
      // doesn't have a default constructor.

      std::pair<iterator, bool> emplace(
        boost::unordered::detail::empty_emplace =
          boost::unordered::detail::empty_emplace(),
        value_type v = value_type())
      {
        return this->emplace(boost::move(v));
      }

#endif

      template <typename A0>
      std::pair<iterator, bool> emplace(BOOST_FWD_REF(A0) a0)
      {
        return table_.emplace_unique(
          table::extractor::extract(boost::forward<A0>(a0)),
          boost::unordered::detail::create_emplace_args(
            boost::forward<A0>(a0)));
      }

      template <typename A0, typename A1>
      std::pair<iterator, bool> emplace(
        BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1)
      {
        return table_.emplace_unique(
          table::extractor::extract(
            boost::forward<A0>(a0), boost::forward<A1>(a1)),
          boost::unordered::detail::create_emplace_args(
            boost::forward<A0>(a0), boost::forward<A1>(a1)));
      }

      template <typename A0, typename A1, typename A2>
      std::pair<iterator, bool> emplace(
        BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1, BOOST_FWD_REF(A2) a2)
      {
        return table_.emplace_unique(
          table::extractor::extract(
            boost::forward<A0>(a0), boost::forward<A1>(a1)),
          boost::unordered::detail::create_emplace_args(boost::forward<A0>(a0),
            boost::forward<A1>(a1), boost::forward<A2>(a2)));
      }

#endif

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

      template <class... Args>
      iterator emplace_hint(const_iterator hint, BOOST_FWD_REF(Args)... args)
      {
        return table_.emplace_hint_unique(hint,
          table::extractor::extract(boost::forward<Args>(args)...),
          boost::forward<Args>(args)...);
      }

#else

#if !BOOST_UNORDERED_SUN_WORKAROUNDS1

      iterator emplace_hint(const_iterator hint,
        boost::unordered::detail::empty_emplace =
          boost::unordered::detail::empty_emplace(),
        value_type v = value_type())
      {
        return this->emplace_hint(hint, boost::move(v));
      }

#endif

      template <typename A0>
      iterator emplace_hint(const_iterator hint, BOOST_FWD_REF(A0) a0)
      {
        return table_.emplace_hint_unique(hint,
          table::extractor::extract(boost::forward<A0>(a0)),
          boost::unordered::detail::create_emplace_args(
            boost::forward<A0>(a0)));
      }

      template <typename A0, typename A1>
      iterator emplace_hint(
        const_iterator hint, BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1)
      {
        return table_.emplace_hint_unique(hint,
          table::extractor::extract(
            boost::forward<A0>(a0), boost::forward<A1>(a1)),
          boost::unordered::detail::create_emplace_args(
            boost::forward<A0>(a0), boost::forward<A1>(a1)));
      }

      template <typename A0, typename A1, typename A2>
      iterator emplace_hint(const_iterator hint, BOOST_FWD_REF(A0) a0,
        BOOST_FWD_REF(A1) a1, BOOST_FWD_REF(A2) a2)
      {
        return table_.emplace_hint_unique(hint,
          table::extractor::extract(
            boost::forward<A0>(a0), boost::forward<A1>(a1)),
          boost::unordered::detail::create_emplace_args(boost::forward<A0>(a0),
            boost::forward<A1>(a1), boost::forward<A2>(a2)));
      }

#endif

#if defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

#define BOOST_UNORDERED_EMPLACE(z, n, _)                                       \
  template <BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)>                          \
  std::pair<iterator, bool> emplace(                                           \
    BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a))                        \
  {                                                                            \
    return table_.emplace_unique(                                              \
      table::extractor::extract(                                               \
        boost::forward<A0>(a0), boost::forward<A1>(a1)),                       \
      boost::unordered::detail::create_emplace_args(                           \
        BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD, a)));               \
  }                                                                            \
                                                                               \
  template <BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)>                          \
  iterator emplace_hint(                                                       \
    const_iterator hint, BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a))   \
  {                                                                            \
    return table_.emplace_hint_unique(hint,                                    \
      table::extractor::extract(                                               \
        boost::forward<A0>(a0), boost::forward<A1>(a1)),                       \
      boost::unordered::detail::create_emplace_args(                           \
        BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD, a)));               \
  }

      BOOST_UNORDERED_EMPLACE(1, 4, _)
      BOOST_UNORDERED_EMPLACE(1, 5, _)
      BOOST_UNORDERED_EMPLACE(1, 6, _)
      BOOST_UNORDERED_EMPLACE(1, 7, _)
      BOOST_UNORDERED_EMPLACE(1, 8, _)
      BOOST_UNORDERED_EMPLACE(1, 9, _)
      BOOST_PP_REPEAT_FROM_TO(10, BOOST_PP_INC(BOOST_UNORDERED_EMPLACE_LIMIT),
        BOOST_UNORDERED_EMPLACE, _)

#undef BOOST_UNORDERED_EMPLACE

#endif

      std::pair<iterator, bool> insert(value_type const& x)
      {
        return this->emplace(x);
      }

      std::pair<iterator, bool> insert(BOOST_RV_REF(value_type) x)
      {
        return this->emplace(boost::move(x));
      }

      template <class P2>
      typename boost::enable_if<
        boost::is_constructible<value_type, BOOST_RV_REF(P2)>,
        std::pair<iterator, bool> >::type insert(BOOST_RV_REF(P2) obj)
      {
        return this->emplace(boost::forward<P2>(obj));
      }

      iterator insert(const_iterator hint, value_type const& x)
      {
        return this->emplace_hint(hint, x);
      }

      iterator insert(const_iterator hint, BOOST_RV_REF(value_type) x)
      {
        return this->emplace_hint(hint, boost::move(x));
      }

      template <class P2>
      typename boost::enable_if<
        boost::is_constructible<value_type, BOOST_RV_REF(P2)>, iterator>::type
      insert(const_iterator hint, BOOST_RV_REF(P2) obj)
      {
        return this->emplace_hint(hint, boost::forward<P2>(obj));
      }

      template <class InputIt> void insert(InputIt, InputIt);

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
      void insert(std::initializer_list<value_type>);
#endif

      // extract

      node_type extract(const_iterator position)
      {
        return node_type(
          table_.extract_by_iterator_unique(position), table_.node_alloc());
      }

      node_type extract(const key_type& k)
      {
        return node_type(table_.extract_by_key_impl(k), table_.node_alloc());
      }

      template <class Key>
      typename boost::enable_if_c<
        detail::transparent_non_iterable<Key, unordered_map>::value,
        node_type>::type
      extract(BOOST_FWD_REF(Key) k)
      {
        return node_type(table_.extract_by_key_impl(boost::forward<Key>(k)),
          table_.node_alloc());
      }

      insert_return_type insert(BOOST_RV_REF(node_type) np)
      {
        insert_return_type result;
        table_.move_insert_node_type_unique((node_type&)np, result);
        return boost::move(result);
      }

      iterator insert(const_iterator hint, BOOST_RV_REF(node_type) np)
      {
        return table_.move_insert_node_type_with_hint_unique(hint, np);
      }

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES) ||                               \
  (BOOST_COMP_GNUC && BOOST_COMP_GNUC < BOOST_VERSION_NUMBER(4, 6, 0))
    private:
      // Note: Use r-value node_type to insert.
      insert_return_type insert(node_type&);
      iterator insert(const_iterator, node_type& np);

    public:
#endif

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

      template <class... Args>
      std::pair<iterator, bool> try_emplace(
        key_type const& k, BOOST_FWD_REF(Args)... args)
      {
        return table_.try_emplace_unique(k, boost::forward<Args>(args)...);
      }

      template <class... Args>
      std::pair<iterator, bool> try_emplace(
        BOOST_RV_REF(key_type) k, BOOST_FWD_REF(Args)... args)
      {
        return table_.try_emplace_unique(
          boost::move(k), boost::forward<Args>(args)...);
      }

      template <class Key, class... Args>
      typename boost::enable_if_c<
        detail::transparent_non_iterable<Key, unordered_map>::value,
        std::pair<iterator, bool> >::type
      try_emplace(Key&& k, Args&&... args)
      {
        return table_.try_emplace_unique(
          boost::forward<Key>(k), boost::forward<Args>(args)...);
      }

      template <class... Args>
      iterator try_emplace(
        const_iterator hint, key_type const& k, BOOST_FWD_REF(Args)... args)
      {
        return table_.try_emplace_hint_unique(
          hint, k, boost::forward<Args>(args)...);
      }

      template <class... Args>
      iterator try_emplace(const_iterator hint, BOOST_RV_REF(key_type) k,
        BOOST_FWD_REF(Args)... args)
      {
        return table_.try_emplace_hint_unique(
          hint, boost::move(k), boost::forward<Args>(args)...);
      }

      template <class Key, class... Args>
      typename boost::enable_if_c<
        detail::transparent_non_iterable<Key, unordered_map>::value,
        iterator>::type
      try_emplace(const_iterator hint, Key&& k, Args&&... args)
      {
        return table_.try_emplace_hint_unique(
          hint, boost::forward<Key>(k), boost::forward<Args>(args)...);
      }

#else

      // In order to make this a template, this handles both:
      // try_emplace(key const&)
      // try_emplace(key&&)

      template <typename Key>
      std::pair<iterator, bool> try_emplace(BOOST_FWD_REF(Key) k)
      {
        return table_.try_emplace_unique(boost::forward<Key>(k));
      }

      // In order to make this a template, this handles both:
      // try_emplace(const_iterator hint, key const&)
      // try_emplace(const_iterator hint, key&&)

      template <typename Key>
      iterator try_emplace(const_iterator hint, BOOST_FWD_REF(Key) k)
      {
        return table_.try_emplace_hint_unique(hint, boost::forward<Key>(k));
      }

      // try_emplace(key const&, Args&&...)

      template <typename A0>
      std::pair<iterator, bool> try_emplace(
        key_type const& k, BOOST_FWD_REF(A0) a0)
      {
        return table_.try_emplace_unique(
          k, boost::unordered::detail::create_emplace_args(
               boost::forward<A0>(a0)));
      }

      template <typename A0, typename A1>
      std::pair<iterator, bool> try_emplace(
        key_type const& k, BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1)
      {
        return table_.try_emplace_unique(
          k, boost::unordered::detail::create_emplace_args(
               boost::forward<A0>(a0), boost::forward<A1>(a1)));
      }

      template <typename A0, typename A1, typename A2>
      std::pair<iterator, bool> try_emplace(key_type const& k,
        BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1, BOOST_FWD_REF(A2) a2)
      {
        return table_.try_emplace_unique(k,
          boost::unordered::detail::create_emplace_args(boost::forward<A0>(a0),
            boost::forward<A1>(a1), boost::forward<A2>(a2)));
      }

      // try_emplace(key&&, Args&&...)

      template <typename A0>
      std::pair<iterator, bool> try_emplace(
        BOOST_RV_REF(key_type) k, BOOST_FWD_REF(A0) a0)
      {
        return table_.try_emplace_unique(
          boost::move(k), boost::unordered::detail::create_emplace_args(
                            boost::forward<A0>(a0)));
      }

      template <typename A0, typename A1>
      std::pair<iterator, bool> try_emplace(
        BOOST_RV_REF(key_type) k, BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1)
      {
        return table_.try_emplace_unique(
          boost::move(k), boost::unordered::detail::create_emplace_args(
                            boost::forward<A0>(a0), boost::forward<A1>(a1)));
      }

      template <typename A0, typename A1, typename A2>
      std::pair<iterator, bool> try_emplace(BOOST_RV_REF(key_type) k,
        BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1, BOOST_FWD_REF(A2) a2)
      {
        return table_.try_emplace_unique(boost::move(k),
          boost::unordered::detail::create_emplace_args(boost::forward<A0>(a0),
            boost::forward<A1>(a1), boost::forward<A2>(a2)));
      }

      // try_emplace(Key&&, Args&&...)

      template <typename Key, typename A0>
      typename boost::enable_if_c<
        detail::transparent_non_iterable<Key, unordered_map>::value,
        std::pair<iterator, bool> >::type
      try_emplace(BOOST_FWD_REF(Key) k, BOOST_FWD_REF(A0) a0)
      {
        return table_.try_emplace_unique(
          boost::forward<Key>(k), boost::unordered::detail::create_emplace_args(
                                    boost::forward<A0>(a0)));
      }

      template <typename Key, typename A0, typename A1>
      typename boost::enable_if_c<
        detail::transparent_non_iterable<Key, unordered_map>::value,
        std::pair<iterator, bool> >::type
      try_emplace(
        BOOST_FWD_REF(Key) k, BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1)
      {
        return table_.try_emplace_unique(boost::forward<Key>(k),
          boost::unordered::detail::create_emplace_args(
            boost::forward<A0>(a0), boost::forward<A1>(a1)));
      }

      template <typename Key, typename A0, typename A1, typename A2>
      typename boost::enable_if_c<
        detail::transparent_non_iterable<Key, unordered_map>::value,
        std::pair<iterator, bool> >::type
      try_emplace(BOOST_FWD_REF(Key) k, BOOST_FWD_REF(A0) a0,
        BOOST_FWD_REF(A1) a1, BOOST_FWD_REF(A2) a2)
      {
        return table_.try_emplace_unique(boost::forward<Key>(k),
          boost::unordered::detail::create_emplace_args(boost::forward<A0>(a0),
            boost::forward<A1>(a1), boost::forward<A2>(a2)));
      }

      // try_emplace(const_iterator hint, key const&, Args&&...)

      template <typename A0>
      iterator try_emplace(
        const_iterator hint, key_type const& k, BOOST_FWD_REF(A0) a0)
      {
        return table_.try_emplace_hint_unique(
          hint, k, boost::unordered::detail::create_emplace_args(
                     boost::forward<A0>(a0)));
      }

      template <typename A0, typename A1>
      iterator try_emplace(const_iterator hint, key_type const& k,
        BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1)
      {
        return table_.try_emplace_hint_unique(
          hint, k, boost::unordered::detail::create_emplace_args(
                     boost::forward<A0>(a0), boost::forward<A1>(a1)));
      }

      template <typename A0, typename A1, typename A2>
      iterator try_emplace(const_iterator hint, key_type const& k,
        BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1, BOOST_FWD_REF(A2) a2)
      {
        return table_.try_emplace_hint_unique(hint, k,
          boost::unordered::detail::create_emplace_args(boost::forward<A0>(a0),
            boost::forward<A1>(a1), boost::forward<A2>(a2)));
      }

      // try_emplace(const_iterator hint, key&&, Args&&...)

      template <typename A0>
      iterator try_emplace(
        const_iterator hint, BOOST_RV_REF(key_type) k, BOOST_FWD_REF(A0) a0)
      {
        return table_.try_emplace_hint_unique(
          hint, boost::move(k), boost::unordered::detail::create_emplace_args(
                                  boost::forward<A0>(a0)));
      }

      template <typename A0, typename A1>
      iterator try_emplace(const_iterator hint, BOOST_RV_REF(key_type) k,
        BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1)
      {
        return table_.try_emplace_hint_unique(hint, boost::move(k),
          boost::unordered::detail::create_emplace_args(
            boost::forward<A0>(a0), boost::forward<A1>(a1)));
      }

      template <typename A0, typename A1, typename A2>
      iterator try_emplace(const_iterator hint, BOOST_RV_REF(key_type) k,
        BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1, BOOST_FWD_REF(A2) a2)
      {
        return table_.try_emplace_hint_unique(hint, boost::move(k),
          boost::unordered::detail::create_emplace_args(boost::forward<A0>(a0),
            boost::forward<A1>(a1), boost::forward<A2>(a2)));
      }

      // try_emplace(const_iterator hint, Key&&, Args&&...)

      template <typename Key, typename A0>
      typename boost::enable_if_c<
        detail::transparent_non_iterable<Key, unordered_map>::value,
        iterator>::type
      try_emplace(
        const_iterator hint, BOOST_FWD_REF(Key) k, BOOST_FWD_REF(A0) a0)
      {
        return table_.try_emplace_hint_unique(hint, boost::forward<Key>(k),
          boost::unordered::detail::create_emplace_args(
            boost::forward<A0>(a0)));
      }

      template <typename Key, typename A0, typename A1>
      typename boost::enable_if_c<
        detail::transparent_non_iterable<Key, unordered_map>::value,
        iterator>::type
      try_emplace(const_iterator hint, BOOST_FWD_REF(Key) k,
        BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1)
      {
        return table_.try_emplace_hint_unique(hint, boost::forward<Key>(k),
          boost::unordered::detail::create_emplace_args(
            boost::forward<A0>(a0), boost::forward<A1>(a1)));
      }

      template <typename Key, typename A0, typename A1, typename A2>
      typename boost::enable_if_c<
        detail::transparent_non_iterable<Key, unordered_map>::value,
        iterator>::type
      try_emplace(const_iterator hint, BOOST_FWD_REF(Key) k,
        BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1, BOOST_FWD_REF(A2) a2)
      {
        return table_.try_emplace_hint_unique(hint, boost::forward<Key>(k),
          boost::unordered::detail::create_emplace_args(boost::forward<A0>(a0),
            boost::forward<A1>(a1), boost::forward<A2>(a2)));
      }

#define BOOST_UNORDERED_TRY_EMPLACE(z, n, _)                                   \
                                                                               \
  template <BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)>                          \
  std::pair<iterator, bool> try_emplace(                                       \
    key_type const& k, BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a))     \
  {                                                                            \
    return table_.try_emplace_unique(                                          \
      k, boost::unordered::detail::create_emplace_args(                        \
           BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD, a)));            \
  }                                                                            \
                                                                               \
  template <BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)>                          \
  std::pair<iterator, bool> try_emplace(BOOST_RV_REF(key_type) k,              \
    BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a))                        \
  {                                                                            \
    return table_.try_emplace_unique(boost::move(k),                           \
      boost::unordered::detail::create_emplace_args(                           \
        BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD, a)));               \
  }                                                                            \
                                                                               \
  template <BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)>                          \
  iterator try_emplace(const_iterator hint, key_type const& k,                 \
    BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a))                        \
  {                                                                            \
    return table_.try_emplace_hint_unique(                                     \
      hint, k, boost::unordered::detail::create_emplace_args(                  \
                 BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD, a)));      \
  }                                                                            \
                                                                               \
  template <BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)>                          \
  iterator try_emplace(const_iterator hint, BOOST_RV_REF(key_type) k,          \
    BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a))                        \
  {                                                                            \
    return table_.try_emplace_hint_unique(hint, boost::move(k),                \
      boost::unordered::detail::create_emplace_args(                           \
        BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD, a)));               \
  }

      BOOST_UNORDERED_TRY_EMPLACE(1, 4, _)
      BOOST_UNORDERED_TRY_EMPLACE(1, 5, _)
      BOOST_UNORDERED_TRY_EMPLACE(1, 6, _)
      BOOST_UNORDERED_TRY_EMPLACE(1, 7, _)
      BOOST_UNORDERED_TRY_EMPLACE(1, 8, _)
      BOOST_UNORDERED_TRY_EMPLACE(1, 9, _)
      BOOST_PP_REPEAT_FROM_TO(10, BOOST_PP_INC(BOOST_UNORDERED_EMPLACE_LIMIT),
        BOOST_UNORDERED_TRY_EMPLACE, _)

#undef BOOST_UNORDERED_TRY_EMPLACE

#endif

      template <class M>
      std::pair<iterator, bool> insert_or_assign(
        key_type const& k, BOOST_FWD_REF(M) obj)
      {
        return table_.insert_or_assign_unique(k, boost::forward<M>(obj));
      }

      template <class M>
      std::pair<iterator, bool> insert_or_assign(
        BOOST_RV_REF(key_type) k, BOOST_FWD_REF(M) obj)
      {
        return table_.insert_or_assign_unique(
          boost::move(k), boost::forward<M>(obj));
      }

      template <class Key, class M>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        std::pair<iterator, bool> >::type
      insert_or_assign(BOOST_FWD_REF(Key) k, BOOST_FWD_REF(M) obj)
      {
        return table_.insert_or_assign_unique(
          boost::forward<Key>(k), boost::forward<M>(obj));
      }

      template <class M>
      iterator insert_or_assign(
        const_iterator, key_type const& k, BOOST_FWD_REF(M) obj)
      {
        return table_.insert_or_assign_unique(k, boost::forward<M>(obj)).first;
      }

      template <class M>
      iterator insert_or_assign(
        const_iterator, BOOST_RV_REF(key_type) k, BOOST_FWD_REF(M) obj)
      {
        return table_
          .insert_or_assign_unique(boost::move(k), boost::forward<M>(obj))
          .first;
      }

      template <class Key, class M>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        iterator>::type
      insert_or_assign(
        const_iterator, BOOST_FWD_REF(Key) k, BOOST_FWD_REF(M) obj)
      {
        return table_
          .insert_or_assign_unique(
            boost::forward<Key>(k), boost::forward<M>(obj))
          .first;
      }

      iterator erase(iterator);
      iterator erase(const_iterator);
      size_type erase(const key_type&);
      iterator erase(const_iterator, const_iterator);

      template <class Key>
      typename boost::enable_if_c<
        detail::transparent_non_iterable<Key, unordered_map>::value,
        size_type>::type
      erase(BOOST_FWD_REF(Key) k)
      {
        return table_.erase_key_unique_impl(boost::forward<Key>(k));
      }

      BOOST_UNORDERED_DEPRECATED("Use erase instead")
      void quick_erase(const_iterator it) { erase(it); }
      BOOST_UNORDERED_DEPRECATED("Use erase instead")
      void erase_return_void(const_iterator it) { erase(it); }

      void swap(unordered_map&)
        BOOST_NOEXCEPT_IF(value_allocator_traits::is_always_equal::value&&
            boost::is_nothrow_swappable<H>::value&&
              boost::is_nothrow_swappable<P>::value);
      void clear() BOOST_NOEXCEPT { table_.clear_impl(); }

      template <typename H2, typename P2>
      void merge(boost::unordered_map<K, T, H2, P2, A>& source);

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
      template <typename H2, typename P2>
      void merge(boost::unordered_map<K, T, H2, P2, A>&& source);
#endif

      template <typename H2, typename P2>
      void merge(boost::unordered_multimap<K, T, H2, P2, A>& source);

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
      template <typename H2, typename P2>
      void merge(boost::unordered_multimap<K, T, H2, P2, A>&& source);
#endif

      // observers

      hasher hash_function() const;
      key_equal key_eq() const;

      // lookup

      iterator find(const key_type&);
      const_iterator find(const key_type&) const;

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        iterator>::type
      find(const Key& key)
      {
        return table_.find(key);
      }

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        const_iterator>::type
      find(const Key& key) const
      {
        return const_iterator(table_.find(key));
      }

      template <class CompatibleKey, class CompatibleHash,
        class CompatiblePredicate>
      iterator find(CompatibleKey const&, CompatibleHash const&,
        CompatiblePredicate const&);

      template <class CompatibleKey, class CompatibleHash,
        class CompatiblePredicate>
      const_iterator find(CompatibleKey const&, CompatibleHash const&,
        CompatiblePredicate const&) const;

      bool contains(const key_type& k) const
      {
        return table_.find(k) != this->end();
      }

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        bool>::type
      contains(const Key& k) const
      {
        return table_.find(k) != this->end();
      }

      size_type count(const key_type&) const;

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        size_type>::type
      count(const Key& k) const
      {
        return (table_.find(k) != this->end() ? 1 : 0);
      }

      std::pair<iterator, iterator> equal_range(const key_type&);
      std::pair<const_iterator, const_iterator> equal_range(
        const key_type&) const;

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        std::pair<iterator, iterator> >::type
      equal_range(const Key& key)
      {
        iterator first = table_.find(key);
        iterator last = first;
        if (last != this->end()) {
          ++last;
        }

        return std::make_pair(first, last);
      }

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        std::pair<const_iterator, const_iterator> >::type
      equal_range(const Key& key) const
      {
        iterator first = table_.find(key);
        iterator last = first;
        if (last != this->end()) {
          ++last;
        }

        return std::make_pair(first, last);
      }

      mapped_type& operator[](const key_type&);
      mapped_type& operator[](BOOST_RV_REF(key_type));

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        mapped_type&>::type
      operator[](BOOST_FWD_REF(Key) k);

      mapped_type& at(const key_type&);
      mapped_type const& at(const key_type&) const;

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        mapped_type&>::type at(BOOST_FWD_REF(Key) k);

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        mapped_type const&>::type at(BOOST_FWD_REF(Key) k) const;

      // bucket interface

      size_type bucket_count() const BOOST_NOEXCEPT
      {
        return table_.bucket_count();
      }

      size_type max_bucket_count() const BOOST_NOEXCEPT
      {
        return table_.max_bucket_count();
      }

      size_type bucket_size(size_type) const;

      size_type bucket(const key_type& k) const
      {
        return table_.hash_to_bucket(table_.hash(k));
      }

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        size_type>::type
      bucket(BOOST_FWD_REF(Key) k) const
      {
        return table_.hash_to_bucket(table_.hash(boost::forward<Key>(k)));
      }

      local_iterator begin(size_type n)
      {
        return table_.begin(n);
      }

      const_local_iterator begin(size_type n) const
      {
        return const_local_iterator(table_.begin(n));
      }

      local_iterator end(size_type) { return local_iterator(); }
      const_local_iterator end(size_type) const
      {
        return const_local_iterator();
      }

      const_local_iterator cbegin(size_type n) const
      {
        return const_local_iterator(table_.begin(n));
      }

      const_local_iterator cend(size_type) const
      {
        return const_local_iterator();
      }

      // hash policy

      float load_factor() const BOOST_NOEXCEPT;
      float max_load_factor() const BOOST_NOEXCEPT { return table_.mlf_; }
      void max_load_factor(float) BOOST_NOEXCEPT;
      void rehash(size_type);
      void reserve(size_type);

#if !BOOST_WORKAROUND(BOOST_BORLANDC, < 0x0582)
      friend bool operator==
        <K, T, H, P, A>(unordered_map const&, unordered_map const&);
      friend bool operator!=
        <K, T, H, P, A>(unordered_map const&, unordered_map const&);
#endif
    }; // class template unordered_map

#if BOOST_UNORDERED_TEMPLATE_DEDUCTION_GUIDES

    template <class InputIterator,
      class Hash =
        boost::hash<boost::unordered::detail::iter_key_t<InputIterator> >,
      class Pred =
        std::equal_to<boost::unordered::detail::iter_key_t<InputIterator> >,
      class Allocator = std::allocator<
        boost::unordered::detail::iter_to_alloc_t<InputIterator> >,
      class = boost::enable_if_t<detail::is_input_iterator_v<InputIterator> >,
      class = boost::enable_if_t<detail::is_hash_v<Hash> >,
      class = boost::enable_if_t<detail::is_pred_v<Pred> >,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_map(InputIterator, InputIterator,
      std::size_t = boost::unordered::detail::default_bucket_count,
      Hash = Hash(), Pred = Pred(), Allocator = Allocator())
      -> unordered_map<boost::unordered::detail::iter_key_t<InputIterator>,
        boost::unordered::detail::iter_val_t<InputIterator>, Hash, Pred,
        Allocator>;

    template <class Key, class T,
      class Hash = boost::hash<boost::remove_const_t<Key> >,
      class Pred = std::equal_to<boost::remove_const_t<Key> >,
      class Allocator = std::allocator<std::pair<const Key, T> >,
      class = boost::enable_if_t<detail::is_hash_v<Hash> >,
      class = boost::enable_if_t<detail::is_pred_v<Pred> >,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_map(std::initializer_list<std::pair<Key, T> >,
      std::size_t = boost::unordered::detail::default_bucket_count,
      Hash = Hash(), Pred = Pred(), Allocator = Allocator())
      -> unordered_map<boost::remove_const_t<Key>, T, Hash, Pred, Allocator>;

    template <class InputIterator, class Allocator,
      class = boost::enable_if_t<detail::is_input_iterator_v<InputIterator> >,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_map(InputIterator, InputIterator, std::size_t, Allocator)
      -> unordered_map<boost::unordered::detail::iter_key_t<InputIterator>,
        boost::unordered::detail::iter_val_t<InputIterator>,
        boost::hash<boost::unordered::detail::iter_key_t<InputIterator> >,
        std::equal_to<boost::unordered::detail::iter_key_t<InputIterator> >,
        Allocator>;

    template <class InputIterator, class Allocator,
      class = boost::enable_if_t<detail::is_input_iterator_v<InputIterator> >,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_map(InputIterator, InputIterator, Allocator)
      -> unordered_map<boost::unordered::detail::iter_key_t<InputIterator>,
        boost::unordered::detail::iter_val_t<InputIterator>,
        boost::hash<boost::unordered::detail::iter_key_t<InputIterator> >,
        std::equal_to<boost::unordered::detail::iter_key_t<InputIterator> >,
        Allocator>;

    template <class InputIterator, class Hash, class Allocator,
      class = boost::enable_if_t<detail::is_hash_v<Hash> >,
      class = boost::enable_if_t<detail::is_input_iterator_v<InputIterator> >,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_map(InputIterator, InputIterator, std::size_t, Hash, Allocator)
      -> unordered_map<boost::unordered::detail::iter_key_t<InputIterator>,
        boost::unordered::detail::iter_val_t<InputIterator>, Hash,
        std::equal_to<boost::unordered::detail::iter_key_t<InputIterator> >,
        Allocator>;

    template <class Key, class T, class Allocator,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_map(std::initializer_list<std::pair<Key, T> >, std::size_t,
      Allocator) -> unordered_map<boost::remove_const_t<Key>, T,
      boost::hash<boost::remove_const_t<Key> >,
      std::equal_to<boost::remove_const_t<Key> >, Allocator>;

    template <class Key, class T, class Allocator,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_map(std::initializer_list<std::pair<Key, T> >, Allocator)
      -> unordered_map<boost::remove_const_t<Key>, T,
        boost::hash<boost::remove_const_t<Key> >,
        std::equal_to<boost::remove_const_t<Key> >, Allocator>;

    template <class Key, class T, class Hash, class Allocator,
      class = boost::enable_if_t<detail::is_hash_v<Hash> >,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_map(std::initializer_list<std::pair<Key, T> >, std::size_t, Hash,
      Allocator) -> unordered_map<boost::remove_const_t<Key>, T, Hash,
      std::equal_to<boost::remove_const_t<Key> >, Allocator>;

#endif

    template <class K, class T, class H, class P, class A>
    class unordered_multimap
    {
#if defined(BOOST_UNORDERED_USE_MOVE)
      BOOST_COPYABLE_AND_MOVABLE(unordered_multimap)
#endif
      template <typename, typename, typename, typename, typename>
      friend class unordered_map;

    public:
      typedef K key_type;
      typedef T mapped_type;
      typedef std::pair<const K, T> value_type;
      typedef typename boost::type_identity<H>::type hasher;
      typedef typename boost::type_identity<P>::type key_equal;
      typedef typename boost::type_identity<A>::type allocator_type;

    private:
      typedef boost::unordered::detail::map<A, K, T, H, P> types;
      typedef typename types::value_allocator_traits value_allocator_traits;
      typedef typename types::table table;

    public:
      typedef typename value_allocator_traits::pointer pointer;
      typedef typename value_allocator_traits::const_pointer const_pointer;

      typedef value_type& reference;
      typedef value_type const& const_reference;

      typedef std::size_t size_type;
      typedef std::ptrdiff_t difference_type;

      typedef typename table::iterator iterator;
      typedef typename table::c_iterator const_iterator;
      typedef typename table::l_iterator local_iterator;
      typedef typename table::cl_iterator const_local_iterator;
      typedef typename types::node_type node_type;

    private:
      table table_;

    public:
      // constructors

      unordered_multimap();

      explicit unordered_multimap(size_type, const hasher& = hasher(),
        const key_equal& = key_equal(),
        const allocator_type& = allocator_type());

      template <class InputIt>
      unordered_multimap(InputIt, InputIt,
        size_type = boost::unordered::detail::default_bucket_count,
        const hasher& = hasher(), const key_equal& = key_equal(),
        const allocator_type& = allocator_type());

      unordered_multimap(unordered_multimap const&);

#if defined(BOOST_UNORDERED_USE_MOVE) ||                                       \
  !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
      unordered_multimap(BOOST_RV_REF(unordered_multimap) other)
        BOOST_NOEXCEPT_IF(table::nothrow_move_constructible)
          : table_(other.table_, boost::unordered::detail::move_tag())
      {
        // The move is done in table_
      }
#endif

      explicit unordered_multimap(allocator_type const&);

      unordered_multimap(unordered_multimap const&, allocator_type const&);

      unordered_multimap(
        BOOST_RV_REF(unordered_multimap), allocator_type const&);

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
      unordered_multimap(std::initializer_list<value_type>,
        size_type = boost::unordered::detail::default_bucket_count,
        const hasher& = hasher(), const key_equal& l = key_equal(),
        const allocator_type& = allocator_type());
#endif

      explicit unordered_multimap(size_type, const allocator_type&);

      explicit unordered_multimap(
        size_type, const hasher&, const allocator_type&);

      template <class InputIterator>
      unordered_multimap(InputIterator, InputIterator, const allocator_type&);

      template <class InputIt>
      unordered_multimap(InputIt, InputIt, size_type, const allocator_type&);

      template <class InputIt>
      unordered_multimap(
        InputIt, InputIt, size_type, const hasher&, const allocator_type&);

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
      unordered_multimap(
        std::initializer_list<value_type>, const allocator_type&);

      unordered_multimap(
        std::initializer_list<value_type>, size_type, const allocator_type&);

      unordered_multimap(std::initializer_list<value_type>, size_type,
        const hasher&, const allocator_type&);
#endif

      // Destructor

      ~unordered_multimap() BOOST_NOEXCEPT;

// Assign

#if defined(BOOST_UNORDERED_USE_MOVE)
      unordered_multimap& operator=(BOOST_COPY_ASSIGN_REF(unordered_multimap) x)
      {
        table_.assign(x.table_, boost::unordered::detail::false_type());
        return *this;
      }

      unordered_multimap& operator=(BOOST_RV_REF(unordered_multimap) x)
        BOOST_NOEXCEPT_IF(value_allocator_traits::is_always_equal::value&&
            boost::is_nothrow_move_assignable<H>::value&&
              boost::is_nothrow_move_assignable<P>::value)
      {
        table_.move_assign(x.table_, boost::unordered::detail::false_type());
        return *this;
      }
#else
      unordered_multimap& operator=(unordered_multimap const& x)
      {
        table_.assign(x.table_, boost::unordered::detail::false_type());
        return *this;
      }

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
      unordered_multimap& operator=(unordered_multimap&& x)
        BOOST_NOEXCEPT_IF(value_allocator_traits::is_always_equal::value&&
            boost::is_nothrow_move_assignable<H>::value&&
              boost::is_nothrow_move_assignable<P>::value)
      {
        table_.move_assign(x.table_, boost::unordered::detail::false_type());
        return *this;
      }
#endif
#endif

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
      unordered_multimap& operator=(std::initializer_list<value_type>);
#endif

      allocator_type get_allocator() const BOOST_NOEXCEPT
      {
        return table_.node_alloc();
      }

      // iterators

      iterator begin() BOOST_NOEXCEPT { return iterator(table_.begin()); }

      const_iterator begin() const BOOST_NOEXCEPT
      {
        return const_iterator(table_.begin());
      }

      iterator end() BOOST_NOEXCEPT { return iterator(); }

      const_iterator end() const BOOST_NOEXCEPT { return const_iterator(); }

      const_iterator cbegin() const BOOST_NOEXCEPT
      {
        return const_iterator(table_.begin());
      }

      const_iterator cend() const BOOST_NOEXCEPT { return const_iterator(); }

      // size and capacity

      BOOST_ATTRIBUTE_NODISCARD bool empty() const BOOST_NOEXCEPT
      {
        return table_.size_ == 0;
      }

      size_type size() const BOOST_NOEXCEPT { return table_.size_; }

      size_type max_size() const BOOST_NOEXCEPT;

// emplace

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

      template <class... Args> iterator emplace(BOOST_FWD_REF(Args)... args)
      {
        return iterator(table_.emplace_equiv(
          boost::unordered::detail::func::construct_node_from_args(
            table_.node_alloc(), boost::forward<Args>(args)...)));
      }

#else

#if !BOOST_UNORDERED_SUN_WORKAROUNDS1

      // 0 argument emplace requires special treatment in case
      // the container is instantiated with a value type that
      // doesn't have a default constructor.

      iterator emplace(boost::unordered::detail::empty_emplace =
                         boost::unordered::detail::empty_emplace(),
        value_type v = value_type())
      {
        return this->emplace(boost::move(v));
      }

#endif

      template <typename A0> iterator emplace(BOOST_FWD_REF(A0) a0)
      {
        return iterator(table_.emplace_equiv(
          boost::unordered::detail::func::construct_node_from_args(
            table_.node_alloc(), boost::unordered::detail::create_emplace_args(
                                   boost::forward<A0>(a0)))));
      }

      template <typename A0, typename A1>
      iterator emplace(BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1)
      {
        return iterator(table_.emplace_equiv(
          boost::unordered::detail::func::construct_node_from_args(
            table_.node_alloc(),
            boost::unordered::detail::create_emplace_args(
              boost::forward<A0>(a0), boost::forward<A1>(a1)))));
      }

      template <typename A0, typename A1, typename A2>
      iterator emplace(
        BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1, BOOST_FWD_REF(A2) a2)
      {
        return iterator(table_.emplace_equiv(
          boost::unordered::detail::func::construct_node_from_args(
            table_.node_alloc(),
            boost::unordered::detail::create_emplace_args(
              boost::forward<A0>(a0), boost::forward<A1>(a1),
              boost::forward<A2>(a2)))));
      }

#endif

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

      template <class... Args>
      iterator emplace_hint(const_iterator hint, BOOST_FWD_REF(Args)... args)
      {
        return iterator(table_.emplace_hint_equiv(
          hint, boost::unordered::detail::func::construct_node_from_args(
                  table_.node_alloc(), boost::forward<Args>(args)...)));
      }

#else

#if !BOOST_UNORDERED_SUN_WORKAROUNDS1

      iterator emplace_hint(const_iterator hint,
        boost::unordered::detail::empty_emplace =
          boost::unordered::detail::empty_emplace(),
        value_type v = value_type())
      {
        return this->emplace_hint(hint, boost::move(v));
      }

#endif

      template <typename A0>
      iterator emplace_hint(const_iterator hint, BOOST_FWD_REF(A0) a0)
      {
        return iterator(table_.emplace_hint_equiv(hint,
          boost::unordered::detail::func::construct_node_from_args(
            table_.node_alloc(), boost::unordered::detail::create_emplace_args(
                                   boost::forward<A0>(a0)))));
      }

      template <typename A0, typename A1>
      iterator emplace_hint(
        const_iterator hint, BOOST_FWD_REF(A0) a0, BOOST_FWD_REF(A1) a1)
      {
        return iterator(table_.emplace_hint_equiv(
          hint, boost::unordered::detail::func::construct_node_from_args(
                  table_.node_alloc(),
                  boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0), boost::forward<A1>(a1)))));
      }

      template <typename A0, typename A1, typename A2>
      iterator emplace_hint(const_iterator hint, BOOST_FWD_REF(A0) a0,
        BOOST_FWD_REF(A1) a1, BOOST_FWD_REF(A2) a2)
      {
        return iterator(table_.emplace_hint_equiv(
          hint, boost::unordered::detail::func::construct_node_from_args(
                  table_.node_alloc(),
                  boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0), boost::forward<A1>(a1),
                    boost::forward<A2>(a2)))));
      }

#endif

#if defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

#define BOOST_UNORDERED_EMPLACE(z, n, _)                                       \
  template <BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)>                          \
  iterator emplace(BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a))         \
  {                                                                            \
    return iterator(table_.emplace_equiv(                                      \
      boost::unordered::detail::func::construct_node_from_args(                \
        table_.node_alloc(),                                                   \
        boost::unordered::detail::create_emplace_args(                         \
          BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD, a)))));           \
  }                                                                            \
                                                                               \
  template <BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)>                          \
  iterator emplace_hint(                                                       \
    const_iterator hint, BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a))   \
  {                                                                            \
    return iterator(table_.emplace_hint_equiv(                                 \
      hint, boost::unordered::detail::func::construct_node_from_args(          \
              table_.node_alloc(),                                             \
              boost::unordered::detail::create_emplace_args(                   \
                BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD, a)))));     \
  }

      BOOST_UNORDERED_EMPLACE(1, 4, _)
      BOOST_UNORDERED_EMPLACE(1, 5, _)
      BOOST_UNORDERED_EMPLACE(1, 6, _)
      BOOST_UNORDERED_EMPLACE(1, 7, _)
      BOOST_UNORDERED_EMPLACE(1, 8, _)
      BOOST_UNORDERED_EMPLACE(1, 9, _)
      BOOST_PP_REPEAT_FROM_TO(10, BOOST_PP_INC(BOOST_UNORDERED_EMPLACE_LIMIT),
        BOOST_UNORDERED_EMPLACE, _)

#undef BOOST_UNORDERED_EMPLACE

#endif

      iterator insert(value_type const& x) { return this->emplace(x); }

      iterator insert(BOOST_RV_REF(value_type) x)
      {
        return this->emplace(boost::move(x));
      }

      template <class P2>
      typename boost::enable_if<
        boost::is_constructible<value_type, BOOST_RV_REF(P2)>,
        iterator>::type insert(BOOST_RV_REF(P2) obj)
      {
        return this->emplace(boost::forward<P2>(obj));
      }

      iterator insert(const_iterator hint, value_type const& x)
      {
        return this->emplace_hint(hint, x);
      }

      iterator insert(const_iterator hint, BOOST_RV_REF(value_type) x)
      {
        return this->emplace_hint(hint, boost::move(x));
      }

      template <class P2>
      typename boost::enable_if<
        boost::is_constructible<value_type, BOOST_RV_REF(P2)>,
        iterator>::type
      insert(const_iterator hint, BOOST_RV_REF(P2) obj)
      {
        return this->emplace_hint(hint, boost::forward<P2>(obj));
      }

      template <class InputIt> void insert(InputIt, InputIt);

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
      void insert(std::initializer_list<value_type>);
#endif

      // extract

      node_type extract(const_iterator position)
      {
        return node_type(
          table_.extract_by_iterator_equiv(position), table_.node_alloc());
      }

      node_type extract(const key_type& k)
      {
        return node_type(table_.extract_by_key_impl(k), table_.node_alloc());
      }

      template <class Key>
      typename boost::enable_if_c<
        detail::transparent_non_iterable<Key, unordered_multimap>::value,
        node_type>::type
      extract(const Key& k)
      {
        return node_type(table_.extract_by_key_impl(k), table_.node_alloc());
      }

      iterator insert(BOOST_RV_REF(node_type) np)
      {
        return table_.move_insert_node_type_equiv(np);
      }

      iterator insert(const_iterator hint, BOOST_RV_REF(node_type) np)
      {
        return table_.move_insert_node_type_with_hint_equiv(hint, np);
      }

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES) ||                               \
  (BOOST_COMP_GNUC && BOOST_COMP_GNUC < BOOST_VERSION_NUMBER(4, 6, 0))
    private:
      // Note: Use r-value node_type to insert.
      iterator insert(node_type&);
      iterator insert(const_iterator, node_type& np);

    public:
#endif

      iterator erase(iterator);
      iterator erase(const_iterator);
      size_type erase(const key_type&);
      iterator erase(const_iterator, const_iterator);

      template <class Key>
      typename boost::enable_if_c<
        detail::transparent_non_iterable<Key, unordered_multimap>::value,
        size_type>::type
      erase(BOOST_FWD_REF(Key) k)
      {
        return table_.erase_key_equiv_impl(boost::forward<Key>(k));
      }

      BOOST_UNORDERED_DEPRECATED("Use erase instead")
      void quick_erase(const_iterator it) { erase(it); }
      BOOST_UNORDERED_DEPRECATED("Use erase instead")
      void erase_return_void(const_iterator it) { erase(it); }

      void swap(unordered_multimap&)
        BOOST_NOEXCEPT_IF(value_allocator_traits::is_always_equal::value&&
            boost::is_nothrow_swappable<H>::value&&
              boost::is_nothrow_swappable<P>::value);
      void clear() BOOST_NOEXCEPT { table_.clear_impl(); }

      template <typename H2, typename P2>
      void merge(boost::unordered_multimap<K, T, H2, P2, A>& source);

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
      template <typename H2, typename P2>
      void merge(boost::unordered_multimap<K, T, H2, P2, A>&& source);
#endif

      template <typename H2, typename P2>
      void merge(boost::unordered_map<K, T, H2, P2, A>& source);

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
      template <typename H2, typename P2>
      void merge(boost::unordered_map<K, T, H2, P2, A>&& source);
#endif

      // observers

      hasher hash_function() const;
      key_equal key_eq() const;

      // lookup

      iterator find(const key_type&);
      const_iterator find(const key_type&) const;

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        iterator>::type
      find(const Key& key)
      {
        return table_.find(key);
      }

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        const_iterator>::type
      find(const Key& key) const
      {
        return const_iterator(table_.find(key));
      }

      template <class CompatibleKey, class CompatibleHash,
        class CompatiblePredicate>
      iterator find(CompatibleKey const&, CompatibleHash const&,
        CompatiblePredicate const&);

      template <class CompatibleKey, class CompatibleHash,
        class CompatiblePredicate>
      const_iterator find(CompatibleKey const&, CompatibleHash const&,
        CompatiblePredicate const&) const;

      bool contains(key_type const& k) const
      {
        return table_.find(k) != this->end();
      }

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        bool>::type
      contains(const Key& k) const
      {
        return table_.find(k) != this->end();
      }

      size_type count(const key_type&) const;

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        size_type>::type
      count(const Key& k) const
      {
        return table_.group_count(k);
      }

      std::pair<iterator, iterator> equal_range(const key_type&);
      std::pair<const_iterator, const_iterator> equal_range(
        const key_type&) const;

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        std::pair<iterator, iterator> >::type
      equal_range(const Key& key)
      {
        iterator p = table_.find(key);
        return std::make_pair(p, table_.next_group(key, p));
      }

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        std::pair<const_iterator, const_iterator> >::type
      equal_range(const Key& key) const
      {
        iterator p = table_.find(key);
        return std::make_pair(
          const_iterator(p), const_iterator(table_.next_group(key, p)));
      }

      // bucket interface

      size_type bucket_count() const BOOST_NOEXCEPT
      {
        return table_.bucket_count();
      }

      size_type max_bucket_count() const BOOST_NOEXCEPT
      {
        return table_.max_bucket_count();
      }

      size_type bucket_size(size_type) const;

      size_type bucket(const key_type& k) const
      {
        return table_.hash_to_bucket(table_.hash(k));
      }

      template <class Key>
      typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
        size_type>::type
      bucket(BOOST_FWD_REF(Key) k) const
      {
        return table_.hash_to_bucket(table_.hash(boost::forward<Key>(k)));
      }

      local_iterator begin(size_type n)
      {
        return local_iterator(table_.begin(n));
      }

      const_local_iterator begin(size_type n) const
      {
        return const_local_iterator(table_.begin(n));
      }

      local_iterator end(size_type) { return local_iterator(); }

      const_local_iterator end(size_type) const
      {
        return const_local_iterator();
      }

      const_local_iterator cbegin(size_type n) const
      {
        return const_local_iterator(table_.begin(n));
      }

      const_local_iterator cend(size_type) const
      {
        return const_local_iterator();
      }

      // hash policy

      float load_factor() const BOOST_NOEXCEPT;
      float max_load_factor() const BOOST_NOEXCEPT { return table_.mlf_; }
      void max_load_factor(float) BOOST_NOEXCEPT;
      void rehash(size_type);
      void reserve(size_type);

#if !BOOST_WORKAROUND(BOOST_BORLANDC, < 0x0582)
      friend bool operator==
        <K, T, H, P, A>(unordered_multimap const&, unordered_multimap const&);
      friend bool operator!=
        <K, T, H, P, A>(unordered_multimap const&, unordered_multimap const&);
#endif
    }; // class template unordered_multimap

#if BOOST_UNORDERED_TEMPLATE_DEDUCTION_GUIDES

    template <class InputIterator,
      class Hash =
        boost::hash<boost::unordered::detail::iter_key_t<InputIterator> >,
      class Pred =
        std::equal_to<boost::unordered::detail::iter_key_t<InputIterator> >,
      class Allocator = std::allocator<
        boost::unordered::detail::iter_to_alloc_t<InputIterator> >,
      class = boost::enable_if_t<detail::is_input_iterator_v<InputIterator> >,
      class = boost::enable_if_t<detail::is_hash_v<Hash> >,
      class = boost::enable_if_t<detail::is_pred_v<Pred> >,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_multimap(InputIterator, InputIterator,
      std::size_t = boost::unordered::detail::default_bucket_count,
      Hash = Hash(), Pred = Pred(), Allocator = Allocator())
      -> unordered_multimap<boost::unordered::detail::iter_key_t<InputIterator>,
        boost::unordered::detail::iter_val_t<InputIterator>, Hash, Pred,
        Allocator>;

    template <class Key, class T,
      class Hash = boost::hash<boost::remove_const_t<Key> >,
      class Pred = std::equal_to<boost::remove_const_t<Key> >,
      class Allocator = std::allocator<std::pair<const Key, T> >,
      class = boost::enable_if_t<detail::is_hash_v<Hash> >,
      class = boost::enable_if_t<detail::is_pred_v<Pred> >,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_multimap(std::initializer_list<std::pair<Key, T> >,
      std::size_t = boost::unordered::detail::default_bucket_count,
      Hash = Hash(), Pred = Pred(), Allocator = Allocator())
      -> unordered_multimap<boost::remove_const_t<Key>, T, Hash, Pred,
        Allocator>;

    template <class InputIterator, class Allocator,
      class = boost::enable_if_t<detail::is_input_iterator_v<InputIterator> >,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_multimap(InputIterator, InputIterator, std::size_t, Allocator)
      -> unordered_multimap<boost::unordered::detail::iter_key_t<InputIterator>,
        boost::unordered::detail::iter_val_t<InputIterator>,
        boost::hash<boost::unordered::detail::iter_key_t<InputIterator> >,
        std::equal_to<boost::unordered::detail::iter_key_t<InputIterator> >,
        Allocator>;

    template <class InputIterator, class Allocator,
      class = boost::enable_if_t<detail::is_input_iterator_v<InputIterator> >,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_multimap(InputIterator, InputIterator, Allocator)
      -> unordered_multimap<boost::unordered::detail::iter_key_t<InputIterator>,
        boost::unordered::detail::iter_val_t<InputIterator>,
        boost::hash<boost::unordered::detail::iter_key_t<InputIterator> >,
        std::equal_to<boost::unordered::detail::iter_key_t<InputIterator> >,
        Allocator>;

    template <class InputIterator, class Hash, class Allocator,
      class = boost::enable_if_t<detail::is_hash_v<Hash> >,
      class = boost::enable_if_t<detail::is_input_iterator_v<InputIterator> >,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_multimap(
      InputIterator, InputIterator, std::size_t, Hash, Allocator)
      -> unordered_multimap<boost::unordered::detail::iter_key_t<InputIterator>,
        boost::unordered::detail::iter_val_t<InputIterator>, Hash,
        std::equal_to<boost::unordered::detail::iter_key_t<InputIterator> >,
        Allocator>;

    template <class Key, class T, class Allocator,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_multimap(std::initializer_list<std::pair<Key, T> >, std::size_t,
      Allocator) -> unordered_multimap<boost::remove_const_t<Key>, T,
      boost::hash<boost::remove_const_t<Key> >,
      std::equal_to<boost::remove_const_t<Key> >, Allocator>;

    template <class Key, class T, class Allocator,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_multimap(std::initializer_list<std::pair<Key, T> >, Allocator)
      -> unordered_multimap<boost::remove_const_t<Key>, T,
        boost::hash<boost::remove_const_t<Key> >,
        std::equal_to<boost::remove_const_t<Key> >, Allocator>;

    template <class Key, class T, class Hash, class Allocator,
      class = boost::enable_if_t<detail::is_hash_v<Hash> >,
      class = boost::enable_if_t<detail::is_allocator_v<Allocator> > >
    unordered_multimap(std::initializer_list<std::pair<Key, T> >, std::size_t,
      Hash, Allocator) -> unordered_multimap<boost::remove_const_t<Key>, T,
      Hash, std::equal_to<boost::remove_const_t<Key> >, Allocator>;

#endif

    ////////////////////////////////////////////////////////////////////////////

    template <class K, class T, class H, class P, class A>
    unordered_map<K, T, H, P, A>::unordered_map()
    {
    }

    template <class K, class T, class H, class P, class A>
    unordered_map<K, T, H, P, A>::unordered_map(size_type n, const hasher& hf,
      const key_equal& eql, const allocator_type& a)
        : table_(n, hf, eql, a)
    {
    }

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_map<K, T, H, P, A>::unordered_map(InputIt f, InputIt l,
      size_type n, const hasher& hf, const key_equal& eql,
      const allocator_type& a)
        : table_(boost::unordered::detail::initial_size(f, l, n), hf, eql, a)
    {
      this->insert(f, l);
    }

    template <class K, class T, class H, class P, class A>
    unordered_map<K, T, H, P, A>::unordered_map(unordered_map const& other)
        : table_(other.table_,
            unordered_map::value_allocator_traits::
              select_on_container_copy_construction(other.get_allocator()))
    {
      if (other.size()) {
        table_.copy_buckets(
          other.table_, boost::unordered::detail::true_type());
      }
    }

    template <class K, class T, class H, class P, class A>
    unordered_map<K, T, H, P, A>::unordered_map(allocator_type const& a)
        : table_(boost::unordered::detail::default_bucket_count, hasher(),
            key_equal(), a)
    {
    }

    template <class K, class T, class H, class P, class A>
    unordered_map<K, T, H, P, A>::unordered_map(
      unordered_map const& other, allocator_type const& a)
        : table_(other.table_, a)
    {
      if (other.table_.size_) {
        table_.copy_buckets(
          other.table_, boost::unordered::detail::true_type());
      }
    }

    template <class K, class T, class H, class P, class A>
    unordered_map<K, T, H, P, A>::unordered_map(
      BOOST_RV_REF(unordered_map) other, allocator_type const& a)
        : table_(other.table_, a, boost::unordered::detail::move_tag())
    {
      table_.move_construct_buckets(other.table_);
    }

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)

    template <class K, class T, class H, class P, class A>
    unordered_map<K, T, H, P, A>::unordered_map(
      std::initializer_list<value_type> list, size_type n, const hasher& hf,
      const key_equal& eql, const allocator_type& a)
        : table_(
            boost::unordered::detail::initial_size(list.begin(), list.end(), n),
            hf, eql, a)
    {
      this->insert(list.begin(), list.end());
    }

#endif

    template <class K, class T, class H, class P, class A>
    unordered_map<K, T, H, P, A>::unordered_map(
      size_type n, const allocator_type& a)
        : table_(n, hasher(), key_equal(), a)
    {
    }

    template <class K, class T, class H, class P, class A>
    unordered_map<K, T, H, P, A>::unordered_map(
      size_type n, const hasher& hf, const allocator_type& a)
        : table_(n, hf, key_equal(), a)
    {
    }

    template <class K, class T, class H, class P, class A>
    template <class InputIterator>
    unordered_map<K, T, H, P, A>::unordered_map(
      InputIterator f, InputIterator l, const allocator_type& a)
        : table_(boost::unordered::detail::initial_size(
                   f, l, detail::default_bucket_count),
            hasher(), key_equal(), a)
    {
      this->insert(f, l);
    }

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_map<K, T, H, P, A>::unordered_map(
      InputIt f, InputIt l, size_type n, const allocator_type& a)
        : table_(boost::unordered::detail::initial_size(f, l, n), hasher(),
            key_equal(), a)
    {
      this->insert(f, l);
    }

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_map<K, T, H, P, A>::unordered_map(InputIt f, InputIt l,
      size_type n, const hasher& hf, const allocator_type& a)
        : table_(
            boost::unordered::detail::initial_size(f, l, n), hf, key_equal(), a)
    {
      this->insert(f, l);
    }

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)

    template <class K, class T, class H, class P, class A>
    unordered_map<K, T, H, P, A>::unordered_map(
      std::initializer_list<value_type> list, const allocator_type& a)
        : table_(boost::unordered::detail::initial_size(
                   list.begin(), list.end(), detail::default_bucket_count),
            hasher(), key_equal(), a)
    {
      this->insert(list.begin(), list.end());
    }

    template <class K, class T, class H, class P, class A>
    unordered_map<K, T, H, P, A>::unordered_map(
      std::initializer_list<value_type> list, size_type n,
      const allocator_type& a)
        : table_(
            boost::unordered::detail::initial_size(list.begin(), list.end(), n),
            hasher(), key_equal(), a)
    {
      this->insert(list.begin(), list.end());
    }

    template <class K, class T, class H, class P, class A>
    unordered_map<K, T, H, P, A>::unordered_map(
      std::initializer_list<value_type> list, size_type n, const hasher& hf,
      const allocator_type& a)
        : table_(
            boost::unordered::detail::initial_size(list.begin(), list.end(), n),
            hf, key_equal(), a)
    {
      this->insert(list.begin(), list.end());
    }

#endif

    template <class K, class T, class H, class P, class A>
    unordered_map<K, T, H, P, A>::~unordered_map() BOOST_NOEXCEPT
    {
    }

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)

    template <class K, class T, class H, class P, class A>
    unordered_map<K, T, H, P, A>& unordered_map<K, T, H, P, A>::operator=(
      std::initializer_list<value_type> list)
    {
      this->clear();
      this->insert(list.begin(), list.end());
      return *this;
    }

#endif

    // size and capacity

    template <class K, class T, class H, class P, class A>
    std::size_t unordered_map<K, T, H, P, A>::max_size() const BOOST_NOEXCEPT
    {
      using namespace std;

      // size <= mlf_ * count
      return boost::unordered::detail::double_to_size(
               ceil(static_cast<double>(table_.mlf_) *
                    static_cast<double>(table_.max_bucket_count()))) -
             1;
    }

    // modifiers

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    void unordered_map<K, T, H, P, A>::insert(InputIt first, InputIt last)
    {
      if (first != last) {
        table_.insert_range_unique(
          table::extractor::extract(*first), first, last);
      }
    }

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
    template <class K, class T, class H, class P, class A>
    void unordered_map<K, T, H, P, A>::insert(
      std::initializer_list<value_type> list)
    {
      this->insert(list.begin(), list.end());
    }
#endif

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K, T, H, P, A>::iterator
    unordered_map<K, T, H, P, A>::erase(iterator position)
    {
      return table_.erase_node(position);
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K, T, H, P, A>::iterator
    unordered_map<K, T, H, P, A>::erase(const_iterator position)
    {
      return table_.erase_node(position);
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K, T, H, P, A>::size_type
    unordered_map<K, T, H, P, A>::erase(const key_type& k)
    {
      return table_.erase_key_unique_impl(k);
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K, T, H, P, A>::iterator
    unordered_map<K, T, H, P, A>::erase(
      const_iterator first, const_iterator last)
    {
      return table_.erase_nodes_range(first, last);
    }

    template <class K, class T, class H, class P, class A>
    void unordered_map<K, T, H, P, A>::swap(unordered_map& other)
      BOOST_NOEXCEPT_IF(value_allocator_traits::is_always_equal::value&&
          boost::is_nothrow_swappable<H>::value&&
            boost::is_nothrow_swappable<P>::value)
    {
      table_.swap(other.table_);
    }

    template <class K, class T, class H, class P, class A>
    template <typename H2, typename P2>
    void unordered_map<K, T, H, P, A>::merge(
      boost::unordered_map<K, T, H2, P2, A>& source)
    {
      table_.merge_unique(source.table_);
    }

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    template <class K, class T, class H, class P, class A>
    template <typename H2, typename P2>
    void unordered_map<K, T, H, P, A>::merge(
      boost::unordered_map<K, T, H2, P2, A>&& source)
    {
      table_.merge_unique(source.table_);
    }
#endif

    template <class K, class T, class H, class P, class A>
    template <typename H2, typename P2>
    void unordered_map<K, T, H, P, A>::merge(
      boost::unordered_multimap<K, T, H2, P2, A>& source)
    {
      table_.merge_unique(source.table_);
    }

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    template <class K, class T, class H, class P, class A>
    template <typename H2, typename P2>
    void unordered_map<K, T, H, P, A>::merge(
      boost::unordered_multimap<K, T, H2, P2, A>&& source)
    {
      table_.merge_unique(source.table_);
    }
#endif

    // observers

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K, T, H, P, A>::hasher
    unordered_map<K, T, H, P, A>::hash_function() const
    {
      return table_.hash_function();
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K, T, H, P, A>::key_equal
    unordered_map<K, T, H, P, A>::key_eq() const
    {
      return table_.key_eq();
    }

    // lookup

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K, T, H, P, A>::iterator
    unordered_map<K, T, H, P, A>::find(const key_type& k)
    {
      return iterator(table_.find(k));
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K, T, H, P, A>::const_iterator
    unordered_map<K, T, H, P, A>::find(const key_type& k) const
    {
      return const_iterator(table_.find(k));
    }

    template <class K, class T, class H, class P, class A>
    template <class CompatibleKey, class CompatibleHash,
      class CompatiblePredicate>
    typename unordered_map<K, T, H, P, A>::iterator
    unordered_map<K, T, H, P, A>::find(CompatibleKey const& k,
      CompatibleHash const& hash, CompatiblePredicate const& eq)
    {
      return table_.transparent_find(k, hash, eq);
    }

    template <class K, class T, class H, class P, class A>
    template <class CompatibleKey, class CompatibleHash,
      class CompatiblePredicate>
    typename unordered_map<K, T, H, P, A>::const_iterator
    unordered_map<K, T, H, P, A>::find(CompatibleKey const& k,
      CompatibleHash const& hash, CompatiblePredicate const& eq) const
    {
      return table_.transparent_find(k, hash, eq);
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K, T, H, P, A>::size_type
    unordered_map<K, T, H, P, A>::count(const key_type& k) const
    {
      return table_.find_node(k) ? 1 : 0;
    }

    template <class K, class T, class H, class P, class A>
    std::pair<typename unordered_map<K, T, H, P, A>::iterator,
      typename unordered_map<K, T, H, P, A>::iterator>
    unordered_map<K, T, H, P, A>::equal_range(const key_type& k)
    {
      iterator first = table_.find(k);
      iterator second = first;
      if (second != this->end()) {
        ++second;
      }
      return std::make_pair(first, second);
    }

    template <class K, class T, class H, class P, class A>
    std::pair<typename unordered_map<K, T, H, P, A>::const_iterator,
      typename unordered_map<K, T, H, P, A>::const_iterator>
    unordered_map<K, T, H, P, A>::equal_range(const key_type& k) const
    {
      iterator first = table_.find(k);
      iterator second = first;
      if (second != this->end()) {
        ++second;
      }
      return std::make_pair(const_iterator(first), const_iterator(second));
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K, T, H, P, A>::mapped_type&
      unordered_map<K, T, H, P, A>::operator[](const key_type& k)
    {
      return table_.try_emplace_unique(k).first->second;
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K, T, H, P, A>::mapped_type&
      unordered_map<K, T, H, P, A>::operator[](BOOST_RV_REF(key_type) k)
    {
      return table_.try_emplace_unique(boost::move(k)).first->second;
    }

    template <class K, class T, class H, class P, class A>
    template <class Key>
    typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
      typename unordered_map<K, T, H, P, A>::mapped_type&>::type
    unordered_map<K, T, H, P, A>::operator[](BOOST_FWD_REF(Key) k)
    {
      return table_.try_emplace_unique(boost::forward<Key>(k)).first->second;
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K, T, H, P, A>::mapped_type&
    unordered_map<K, T, H, P, A>::at(const key_type& k)
    {
      typedef typename table::node_pointer node_pointer;

      if (table_.size_) {
        node_pointer p = table_.find_node(k);
        if (p)
          return p->value().second;
      }

      boost::throw_exception(
        std::out_of_range("Unable to find key in unordered_map."));
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K, T, H, P, A>::mapped_type const&
    unordered_map<K, T, H, P, A>::at(const key_type& k) const
    {
      typedef typename table::node_pointer node_pointer;

      if (table_.size_) {
        node_pointer p = table_.find_node(k);
        if (p)
          return p->value().second;
      }

      boost::throw_exception(
        std::out_of_range("Unable to find key in unordered_map."));
    }

    template <class K, class T, class H, class P, class A>
    template <class Key>
    typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
      typename unordered_map<K, T, H, P, A>::mapped_type&>::type
    unordered_map<K, T, H, P, A>::at(BOOST_FWD_REF(Key) k)
    {
      typedef typename table::node_pointer node_pointer;

      if (table_.size_) {
        node_pointer p = table_.find_node(boost::forward<Key>(k));
        if (p)
          return p->value().second;
      }

      boost::throw_exception(
        std::out_of_range("Unable to find key in unordered_map."));
    }

    template <class K, class T, class H, class P, class A>
    template <class Key>
    typename boost::enable_if_c<detail::are_transparent<Key, H, P>::value,
      typename unordered_map<K, T, H, P, A>::mapped_type const&>::type
    unordered_map<K, T, H, P, A>::at(BOOST_FWD_REF(Key) k) const
    {
      typedef typename table::node_pointer node_pointer;

      if (table_.size_) {
        node_pointer p = table_.find_node(boost::forward<Key>(k));
        if (p)
          return p->value().second;
      }

      boost::throw_exception(
        std::out_of_range("Unable to find key in unordered_map."));
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K, T, H, P, A>::size_type
    unordered_map<K, T, H, P, A>::bucket_size(size_type n) const
    {
      return table_.bucket_size(n);
    }

    // hash policy

    template <class K, class T, class H, class P, class A>
    float unordered_map<K, T, H, P, A>::load_factor() const BOOST_NOEXCEPT
    {
      if (table_.size_ == 0) {
        return 0.0f;
      }

      BOOST_ASSERT(table_.bucket_count() != 0);
      return static_cast<float>(table_.size_) /
             static_cast<float>(table_.bucket_count());
    }

    template <class K, class T, class H, class P, class A>
    void unordered_map<K, T, H, P, A>::max_load_factor(float m) BOOST_NOEXCEPT
    {
      table_.max_load_factor(m);
    }

    template <class K, class T, class H, class P, class A>
    void unordered_map<K, T, H, P, A>::rehash(size_type n)
    {
      table_.rehash(n);
    }

    template <class K, class T, class H, class P, class A>
    void unordered_map<K, T, H, P, A>::reserve(size_type n)
    {
      table_.reserve(n);
    }

    template <class K, class T, class H, class P, class A>
    inline bool operator==(unordered_map<K, T, H, P, A> const& m1,
      unordered_map<K, T, H, P, A> const& m2)
    {
#if BOOST_WORKAROUND(BOOST_CODEGEARC, BOOST_TESTED_AT(0x0613))
      struct dummy
      {
        unordered_map<K, T, H, P, A> x;
      };
#endif
      return m1.table_.equals_unique(m2.table_);
    }

    template <class K, class T, class H, class P, class A>
    inline bool operator!=(unordered_map<K, T, H, P, A> const& m1,
      unordered_map<K, T, H, P, A> const& m2)
    {
#if BOOST_WORKAROUND(BOOST_CODEGEARC, BOOST_TESTED_AT(0x0613))
      struct dummy
      {
        unordered_map<K, T, H, P, A> x;
      };
#endif
      return !m1.table_.equals_unique(m2.table_);
    }

    template <class K, class T, class H, class P, class A>
    inline void swap(
      unordered_map<K, T, H, P, A>& m1, unordered_map<K, T, H, P, A>& m2)
      BOOST_NOEXCEPT_IF(BOOST_NOEXCEPT_EXPR(m1.swap(m2)))
    {
#if BOOST_WORKAROUND(BOOST_CODEGEARC, BOOST_TESTED_AT(0x0613))
      struct dummy
      {
        unordered_map<K, T, H, P, A> x;
      };
#endif
      m1.swap(m2);
    }

    template <class K, class T, class H, class P, class A, class Predicate>
    typename unordered_map<K, T, H, P, A>::size_type erase_if(
      unordered_map<K, T, H, P, A>& c, Predicate pred)
    {
      return detail::erase_if(c, pred);
    }

    ////////////////////////////////////////////////////////////////////////////

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K, T, H, P, A>::unordered_multimap()
    {
    }

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K, T, H, P, A>::unordered_multimap(size_type n,
      const hasher& hf, const key_equal& eql, const allocator_type& a)
        : table_(n, hf, eql, a)
    {
    }

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_multimap<K, T, H, P, A>::unordered_multimap(InputIt f, InputIt l,
      size_type n, const hasher& hf, const key_equal& eql,
      const allocator_type& a)
        : table_(boost::unordered::detail::initial_size(f, l, n), hf, eql, a)
    {
      this->insert(f, l);
    }

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K, T, H, P, A>::unordered_multimap(
      unordered_multimap const& other)
        : table_(other.table_,
            unordered_multimap::value_allocator_traits::
              select_on_container_copy_construction(other.get_allocator()))
    {
      if (other.table_.size_) {
        table_.copy_buckets(
          other.table_, boost::unordered::detail::false_type());
      }
    }

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K, T, H, P, A>::unordered_multimap(
      allocator_type const& a)
        : table_(boost::unordered::detail::default_bucket_count, hasher(),
            key_equal(), a)
    {
    }

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K, T, H, P, A>::unordered_multimap(
      unordered_multimap const& other, allocator_type const& a)
        : table_(other.table_, a)
    {
      if (other.table_.size_) {
        table_.copy_buckets(
          other.table_, boost::unordered::detail::false_type());
      }
    }

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K, T, H, P, A>::unordered_multimap(
      BOOST_RV_REF(unordered_multimap) other, allocator_type const& a)
        : table_(other.table_, a, boost::unordered::detail::move_tag())
    {
      table_.move_construct_buckets(other.table_);
    }

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K, T, H, P, A>::unordered_multimap(
      std::initializer_list<value_type> list, size_type n, const hasher& hf,
      const key_equal& eql, const allocator_type& a)
        : table_(
            boost::unordered::detail::initial_size(list.begin(), list.end(), n),
            hf, eql, a)
    {
      this->insert(list.begin(), list.end());
    }

#endif

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K, T, H, P, A>::unordered_multimap(
      size_type n, const allocator_type& a)
        : table_(n, hasher(), key_equal(), a)
    {
    }

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K, T, H, P, A>::unordered_multimap(
      size_type n, const hasher& hf, const allocator_type& a)
        : table_(n, hf, key_equal(), a)
    {
    }

    template <class K, class T, class H, class P, class A>
    template <class InputIterator>
    unordered_multimap<K, T, H, P, A>::unordered_multimap(
      InputIterator f, InputIterator l, const allocator_type& a)
        : table_(boost::unordered::detail::initial_size(
                   f, l, detail::default_bucket_count),
            hasher(), key_equal(), a)
    {
      this->insert(f, l);
    }

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_multimap<K, T, H, P, A>::unordered_multimap(
      InputIt f, InputIt l, size_type n, const allocator_type& a)
        : table_(boost::unordered::detail::initial_size(f, l, n), hasher(),
            key_equal(), a)
    {
      this->insert(f, l);
    }

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_multimap<K, T, H, P, A>::unordered_multimap(InputIt f, InputIt l,
      size_type n, const hasher& hf, const allocator_type& a)
        : table_(
            boost::unordered::detail::initial_size(f, l, n), hf, key_equal(), a)
    {
      this->insert(f, l);
    }

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K, T, H, P, A>::unordered_multimap(
      std::initializer_list<value_type> list, const allocator_type& a)
        : table_(boost::unordered::detail::initial_size(
                   list.begin(), list.end(), detail::default_bucket_count),
            hasher(), key_equal(), a)
    {
      this->insert(list.begin(), list.end());
    }

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K, T, H, P, A>::unordered_multimap(
      std::initializer_list<value_type> list, size_type n,
      const allocator_type& a)
        : table_(
            boost::unordered::detail::initial_size(list.begin(), list.end(), n),
            hasher(), key_equal(), a)
    {
      this->insert(list.begin(), list.end());
    }

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K, T, H, P, A>::unordered_multimap(
      std::initializer_list<value_type> list, size_type n, const hasher& hf,
      const allocator_type& a)
        : table_(
            boost::unordered::detail::initial_size(list.begin(), list.end(), n),
            hf, key_equal(), a)
    {
      this->insert(list.begin(), list.end());
    }

#endif

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K, T, H, P, A>::~unordered_multimap() BOOST_NOEXCEPT
    {
    }

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K, T, H, P, A>& unordered_multimap<K, T, H, P, A>::
    operator=(std::initializer_list<value_type> list)
    {
      this->clear();
      this->insert(list.begin(), list.end());
      return *this;
    }

#endif

    // size and capacity

    template <class K, class T, class H, class P, class A>
    std::size_t
    unordered_multimap<K, T, H, P, A>::max_size() const BOOST_NOEXCEPT
    {
      using namespace std;

      // size <= mlf_ * count
      return boost::unordered::detail::double_to_size(
               ceil(static_cast<double>(table_.mlf_) *
                    static_cast<double>(table_.max_bucket_count()))) -
             1;
    }

    // modifiers

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    void unordered_multimap<K, T, H, P, A>::insert(InputIt first, InputIt last)
    {
      table_.insert_range_equiv(first, last);
    }

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
    template <class K, class T, class H, class P, class A>
    void unordered_multimap<K, T, H, P, A>::insert(
      std::initializer_list<value_type> list)
    {
      this->insert(list.begin(), list.end());
    }
#endif

    template <class K, class T, class H, class P, class A>
    typename unordered_multimap<K, T, H, P, A>::iterator
    unordered_multimap<K, T, H, P, A>::erase(iterator position)
    {
      BOOST_ASSERT(position != this->end());
      return table_.erase_node(position);
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_multimap<K, T, H, P, A>::iterator
    unordered_multimap<K, T, H, P, A>::erase(const_iterator position)
    {
      BOOST_ASSERT(position != this->end());
      return table_.erase_node(position);
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_multimap<K, T, H, P, A>::size_type
    unordered_multimap<K, T, H, P, A>::erase(const key_type& k)
    {
      return table_.erase_key_equiv(k);
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_multimap<K, T, H, P, A>::iterator
    unordered_multimap<K, T, H, P, A>::erase(
      const_iterator first, const_iterator last)
    {
      return table_.erase_nodes_range(first, last);
    }

    template <class K, class T, class H, class P, class A>
    void unordered_multimap<K, T, H, P, A>::swap(unordered_multimap& other)
      BOOST_NOEXCEPT_IF(value_allocator_traits::is_always_equal::value&&
          boost::is_nothrow_swappable<H>::value&&
            boost::is_nothrow_swappable<P>::value)
    {
      table_.swap(other.table_);
    }

    // observers

    template <class K, class T, class H, class P, class A>
    typename unordered_multimap<K, T, H, P, A>::hasher
    unordered_multimap<K, T, H, P, A>::hash_function() const
    {
      return table_.hash_function();
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_multimap<K, T, H, P, A>::key_equal
    unordered_multimap<K, T, H, P, A>::key_eq() const
    {
      return table_.key_eq();
    }

    template <class K, class T, class H, class P, class A>
    template <typename H2, typename P2>
    void unordered_multimap<K, T, H, P, A>::merge(
      boost::unordered_multimap<K, T, H2, P2, A>& source)
    {
      while (!source.empty()) {
        insert(source.extract(source.begin()));
      }
    }

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    template <class K, class T, class H, class P, class A>
    template <typename H2, typename P2>
    void unordered_multimap<K, T, H, P, A>::merge(
      boost::unordered_multimap<K, T, H2, P2, A>&& source)
    {
      while (!source.empty()) {
        insert(source.extract(source.begin()));
      }
    }
#endif

    template <class K, class T, class H, class P, class A>
    template <typename H2, typename P2>
    void unordered_multimap<K, T, H, P, A>::merge(
      boost::unordered_map<K, T, H2, P2, A>& source)
    {
      while (!source.empty()) {
        insert(source.extract(source.begin()));
      }
    }

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    template <class K, class T, class H, class P, class A>
    template <typename H2, typename P2>
    void unordered_multimap<K, T, H, P, A>::merge(
      boost::unordered_map<K, T, H2, P2, A>&& source)
    {
      while (!source.empty()) {
        insert(source.extract(source.begin()));
      }
    }
#endif

    // lookup

    template <class K, class T, class H, class P, class A>
    typename unordered_multimap<K, T, H, P, A>::iterator
    unordered_multimap<K, T, H, P, A>::find(const key_type& k)
    {
      return iterator(table_.find(k));
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_multimap<K, T, H, P, A>::const_iterator
    unordered_multimap<K, T, H, P, A>::find(const key_type& k) const
    {
      return const_iterator(table_.find(k));
    }

    template <class K, class T, class H, class P, class A>
    template <class CompatibleKey, class CompatibleHash,
      class CompatiblePredicate>
    typename unordered_multimap<K, T, H, P, A>::iterator
    unordered_multimap<K, T, H, P, A>::find(CompatibleKey const& k,
      CompatibleHash const& hash, CompatiblePredicate const& eq)
    {
      return table_.transparent_find(k, hash, eq);
    }

    template <class K, class T, class H, class P, class A>
    template <class CompatibleKey, class CompatibleHash,
      class CompatiblePredicate>
    typename unordered_multimap<K, T, H, P, A>::const_iterator
    unordered_multimap<K, T, H, P, A>::find(CompatibleKey const& k,
      CompatibleHash const& hash, CompatiblePredicate const& eq) const
    {
      return const_iterator(
        table_.find_node_impl(table::policy::apply_hash(hash, k), k, eq));
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_multimap<K, T, H, P, A>::size_type
    unordered_multimap<K, T, H, P, A>::count(const key_type& k) const
    {
      return table_.group_count(k);
    }

    template <class K, class T, class H, class P, class A>
    std::pair<typename unordered_multimap<K, T, H, P, A>::iterator,
      typename unordered_multimap<K, T, H, P, A>::iterator>
    unordered_multimap<K, T, H, P, A>::equal_range(const key_type& k)
    {
      iterator n = table_.find(k);
      return std::make_pair(n, (n == end() ? n : table_.next_group(k, n)));
    }

    template <class K, class T, class H, class P, class A>
    std::pair<typename unordered_multimap<K, T, H, P, A>::const_iterator,
      typename unordered_multimap<K, T, H, P, A>::const_iterator>
    unordered_multimap<K, T, H, P, A>::equal_range(const key_type& k) const
    {
      iterator n = table_.find(k);
      return std::make_pair(const_iterator(n),
        const_iterator(n == end() ? n : table_.next_group(k, n)));
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_multimap<K, T, H, P, A>::size_type
    unordered_multimap<K, T, H, P, A>::bucket_size(size_type n) const
    {
      return table_.bucket_size(n);
    }

    // hash policy

    template <class K, class T, class H, class P, class A>
    float unordered_multimap<K, T, H, P, A>::load_factor() const BOOST_NOEXCEPT
    {
      if (table_.size_ == 0) {
        return 0.0f;
      }

      BOOST_ASSERT(table_.bucket_count() != 0);
      return static_cast<float>(table_.size_) /
             static_cast<float>(table_.bucket_count());
    }

    template <class K, class T, class H, class P, class A>
    void unordered_multimap<K, T, H, P, A>::max_load_factor(
      float m) BOOST_NOEXCEPT
    {
      table_.max_load_factor(m);
    }

    template <class K, class T, class H, class P, class A>
    void unordered_multimap<K, T, H, P, A>::rehash(size_type n)
    {
      table_.rehash(n);
    }

    template <class K, class T, class H, class P, class A>
    void unordered_multimap<K, T, H, P, A>::reserve(size_type n)
    {
      table_.reserve(n);
    }

    template <class K, class T, class H, class P, class A>
    inline bool operator==(unordered_multimap<K, T, H, P, A> const& m1,
      unordered_multimap<K, T, H, P, A> const& m2)
    {
#if BOOST_WORKAROUND(BOOST_CODEGEARC, BOOST_TESTED_AT(0x0613))
      struct dummy
      {
        unordered_multimap<K, T, H, P, A> x;
      };
#endif
      return m1.table_.equals_equiv(m2.table_);
    }

    template <class K, class T, class H, class P, class A>
    inline bool operator!=(unordered_multimap<K, T, H, P, A> const& m1,
      unordered_multimap<K, T, H, P, A> const& m2)
    {
#if BOOST_WORKAROUND(BOOST_CODEGEARC, BOOST_TESTED_AT(0x0613))
      struct dummy
      {
        unordered_multimap<K, T, H, P, A> x;
      };
#endif
      return !m1.table_.equals_equiv(m2.table_);
    }

    template <class K, class T, class H, class P, class A>
    inline void swap(unordered_multimap<K, T, H, P, A>& m1,
      unordered_multimap<K, T, H, P, A>& m2)
      BOOST_NOEXCEPT_IF(BOOST_NOEXCEPT_EXPR(m1.swap(m2)))
    {
#if BOOST_WORKAROUND(BOOST_CODEGEARC, BOOST_TESTED_AT(0x0613))
      struct dummy
      {
        unordered_multimap<K, T, H, P, A> x;
      };
#endif
      m1.swap(m2);
    }

    template <class K, class T, class H, class P, class A, class Predicate>
    typename unordered_multimap<K, T, H, P, A>::size_type erase_if(
      unordered_multimap<K, T, H, P, A>& c, Predicate pred)
    {
      return detail::erase_if(c, pred);
    }

    template <typename N, class K, class T, class A> class node_handle_map
    {
      BOOST_MOVABLE_BUT_NOT_COPYABLE(node_handle_map)

      template <typename Types> friend struct ::boost::unordered::detail::table;
      template <class K2, class T2, class H2, class P2, class A2>
      friend class boost::unordered::unordered_map;
      template <class K2, class T2, class H2, class P2, class A2>
      friend class boost::unordered::unordered_multimap;

      typedef typename boost::allocator_rebind<A, std::pair<K const, T> >::type
        value_allocator;

      typedef N node;
      typedef typename boost::allocator_rebind<A, node>::type node_allocator;

      typedef
        typename boost::allocator_pointer<node_allocator>::type node_pointer;

    public:
      typedef K key_type;
      typedef T mapped_type;
      typedef A allocator_type;

    private:
      node_pointer ptr_;
      boost::unordered::detail::optional<value_allocator> alloc_;

      node_handle_map(node_pointer ptr, allocator_type const& a)
          : ptr_(ptr), alloc_(a)
      {
      }

    public:
      BOOST_CONSTEXPR node_handle_map() BOOST_NOEXCEPT : ptr_(), alloc_() {}

      ~node_handle_map()
      {
        if (ptr_) {
          node_allocator node_alloc(*alloc_);
          boost::unordered::detail::node_tmp<node_allocator> tmp(
            ptr_, node_alloc);
        }
      }

      node_handle_map(BOOST_RV_REF(node_handle_map) n) BOOST_NOEXCEPT
          : ptr_(n.ptr_),
            alloc_(boost::move(n.alloc_))
      {
        n.ptr_ = node_pointer();
      }

      node_handle_map& operator=(BOOST_RV_REF(node_handle_map) n)
      {
        BOOST_ASSERT(!alloc_.has_value() ||
                     boost::allocator_propagate_on_container_move_assignment<
                       value_allocator>::type::value ||
                     (n.alloc_.has_value() && alloc_ == n.alloc_));

        if (ptr_) {
          node_allocator node_alloc(*alloc_);
          boost::unordered::detail::node_tmp<node_allocator> tmp(
            ptr_, node_alloc);
          ptr_ = node_pointer();
        }

        if (!alloc_.has_value() ||
            boost::allocator_propagate_on_container_move_assignment<
              value_allocator>::type::value) {
          alloc_ = boost::move(n.alloc_);
        }
        ptr_ = n.ptr_;
        n.ptr_ = node_pointer();

        return *this;
      }

      key_type& key() const
      {
        return const_cast<key_type&>(ptr_->value().first);
      }

      mapped_type& mapped() const { return ptr_->value().second; }

      allocator_type get_allocator() const { return *alloc_; }

      BOOST_EXPLICIT_OPERATOR_BOOL_NOEXCEPT()

      bool operator!() const BOOST_NOEXCEPT { return ptr_ ? 0 : 1; }

      BOOST_ATTRIBUTE_NODISCARD bool empty() const BOOST_NOEXCEPT
      {
        return ptr_ ? 0 : 1;
      }

      void swap(node_handle_map& n) BOOST_NOEXCEPT_IF(
        boost::allocator_propagate_on_container_swap<
          value_allocator>::type::value ||
        boost::allocator_is_always_equal<value_allocator>::type::value)
      {

        BOOST_ASSERT(!alloc_.has_value() || !n.alloc_.has_value() ||
                     boost::allocator_propagate_on_container_swap<
                       value_allocator>::type::value ||
                     alloc_ == n.alloc_);
        if (boost::allocator_propagate_on_container_swap<
              value_allocator>::type::value ||
            !alloc_.has_value() || !n.alloc_.has_value()) {
          boost::swap(alloc_, n.alloc_);
        }
        boost::swap(ptr_, n.ptr_);
      }
    };

    template <class N, class K, class T, class A>
    void swap(node_handle_map<N, K, T, A>& x, node_handle_map<N, K, T, A>& y)
      BOOST_NOEXCEPT_IF(BOOST_NOEXCEPT_EXPR(x.swap(y)))
    {
      x.swap(y);
    }

    template <class Iter, class NodeType> struct insert_return_type_map
    {
    private:
      BOOST_MOVABLE_BUT_NOT_COPYABLE(insert_return_type_map)

      // typedef typename boost::allocator_rebind<A,
      //   std::pair<K const, T> >::type value_allocator;
      // typedef N node_;

    public:
      Iter position;
      bool inserted;
      NodeType node;

      insert_return_type_map() : position(), inserted(false), node() {}

      insert_return_type_map(BOOST_RV_REF(insert_return_type_map)
          x) BOOST_NOEXCEPT : position(x.position),
                              inserted(x.inserted),
                              node(boost::move(x.node))
      {
      }

      insert_return_type_map& operator=(BOOST_RV_REF(insert_return_type_map) x)
      {
        inserted = x.inserted;
        position = x.position;
        node = boost::move(x.node);
        return *this;
      }
    };

    template <class Iter, class NodeType>
    void swap(insert_return_type_map<Iter, NodeType>& x,
      insert_return_type_map<Iter, NodeType>& y)
    {
      boost::swap(x.node, y.node);
      boost::swap(x.inserted, y.inserted);
      boost::swap(x.position, y.position);
    }
  } // namespace unordered
} // namespace boost

#if defined(BOOST_MSVC)
#pragma warning(pop)
#endif

#endif // BOOST_UNORDERED_UNORDERED_MAP_HPP_INCLUDED
