
// Copyright 2005-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_UNORDERED_TEST_HELPERS_METAFUNCTIONS_HEADER)
#define BOOST_UNORDERED_TEST_HELPERS_METAFUNCTIONS_HEADER

#include <boost/config.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/not.hpp>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

namespace test
{
    /*
    struct unordered_set_type { char x[100]; };
    struct unordered_multiset_type { char x[200]; };
    struct unordered_map_type { char x[300]; };
    struct unordered_multimap_type { char x[400]; };

    template <class V, class H, class P, class A>
    unordered_set_type container_type(
            boost::unordered_set<V, H, P, A> const*);
    template <class V, class H, class P, class A>
    unordered_multiset_type container_type(
            boost::unordered_multiset<V, H, P, A> const*);
    template <class K, class M, class H, class P, class A>
    unordered_map_type container_type(
            boost::unordered_map<K, M, H, P, A> const*);
    template <class K, class M, class H, class P, class A>
    unordered_multimap_type container_type(
            boost::unordered_multimap<K, M, H, P, A> const*);
    */

    template <class Container>
    struct is_set
        : public boost::is_same<
            BOOST_DEDUCED_TYPENAME Container::key_type,
            BOOST_DEDUCED_TYPENAME Container::value_type> {};

    template <class Container>
    struct is_map
        : public boost::mpl::not_<is_set<Container> > {};

    struct yes_type { char x[100]; };
    struct no_type { char x[200]; };

    template <class V, class H, class P, class A>
    yes_type has_unique_key_impl(
            boost::unordered_set<V, H, P, A> const*);
    template <class V, class H, class P, class A>
    no_type has_unique_key_impl(
            boost::unordered_multiset<V, H, P, A> const*);
    template <class K, class M, class H, class P, class A>
    yes_type has_unique_key_impl(
            boost::unordered_map<K, M, H, P, A> const*);
    template <class K, class M, class H, class P, class A>
    no_type has_unique_key_impl(
            boost::unordered_multimap<K, M, H, P, A> const*);

    template <class Container>
    struct has_unique_keys
    {
        BOOST_STATIC_CONSTANT(bool, value = 
            sizeof(has_unique_key_impl((Container const*)0))
                == sizeof(yes_type));
    };

    template <class Container>
    struct has_equivalent_keys
    {
        BOOST_STATIC_CONSTANT(bool, value = 
            sizeof(has_unique_key_impl((Container const*)0))
                == sizeof(no_type));
    };
}

#endif

