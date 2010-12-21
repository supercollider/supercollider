
// Copyright 2006-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include "../helpers/test.hpp"
#include "../objects/test.hpp"
#include "../helpers/random_values.hpp"
#include "../helpers/tracker.hpp"
#include "../helpers/equivalent.hpp"
#include "../helpers/input_iterator.hpp"
#include "../helpers/invariants.hpp"

#include <iostream>

namespace constructor_tests {

test::seed_t seed(356730);

template <class T>
void constructor_tests1(T*, test::random_generator generator = test::default_generator)
{
    BOOST_DEDUCED_TYPENAME T::hasher hf;
    BOOST_DEDUCED_TYPENAME T::key_equal eq;
    BOOST_DEDUCED_TYPENAME T::allocator_type al;

    std::cerr<<"Construct 1\n";
    {
        T x(0, hf, eq);
        BOOST_CHECK(x.empty());
        BOOST_CHECK(test::equivalent(x.hash_function(), hf));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq));
        BOOST_CHECK(test::equivalent(x.get_allocator(), al));
        test::check_equivalent_keys(x);
    }

    std::cerr<<"Construct 2\n";
    {
        T x(100, hf);
        BOOST_CHECK(x.empty());
        BOOST_CHECK(x.bucket_count() >= 100);
        BOOST_CHECK(test::equivalent(x.hash_function(), hf));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq));
        BOOST_CHECK(test::equivalent(x.get_allocator(), al));
        test::check_equivalent_keys(x);
    }

    std::cerr<<"Construct 3\n";
    {
        T x(2000);
        BOOST_CHECK(x.empty());
        BOOST_CHECK(x.bucket_count() >= 2000);
        BOOST_CHECK(test::equivalent(x.hash_function(), hf));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq));
        BOOST_CHECK(test::equivalent(x.get_allocator(), al));
        test::check_equivalent_keys(x);
    }

    std::cerr<<"Construct 4\n";
    {
        T x;
        BOOST_CHECK(x.empty());
        BOOST_CHECK(test::equivalent(x.hash_function(), hf));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq));
        BOOST_CHECK(test::equivalent(x.get_allocator(), al));
        test::check_equivalent_keys(x);
    }

    std::cerr<<"Construct 5\n";
    {
        test::random_values<T> v(1000, generator);
        T x(v.begin(), v.end(), 10000, hf, eq);
        BOOST_CHECK(x.bucket_count() >= 10000);
        BOOST_CHECK(test::equivalent(x.hash_function(), hf));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq));
        BOOST_CHECK(test::equivalent(x.get_allocator(), al));
        test::check_container(x, v);
        test::check_equivalent_keys(x);
    }

    std::cerr<<"Construct 6\n";
    {
        test::random_values<T> v(10, generator);
        T x(v.begin(), v.end(), 10000, hf);
        BOOST_CHECK(x.bucket_count() >= 10000);
        BOOST_CHECK(test::equivalent(x.hash_function(), hf));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq));
        BOOST_CHECK(test::equivalent(x.get_allocator(), al));
        test::check_container(x, v);
        test::check_equivalent_keys(x);
    }

    std::cerr<<"Construct 7\n";
    {
        test::random_values<T> v(100, generator);
        T x(v.begin(), v.end(), 100);
        BOOST_CHECK(x.bucket_count() >= 100);
        BOOST_CHECK(test::equivalent(x.hash_function(), hf));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq));
        BOOST_CHECK(test::equivalent(x.get_allocator(), al));
        test::check_container(x, v);
        test::check_equivalent_keys(x);
    }

    std::cerr<<"Construct 8\n";
    {
        test::random_values<T> v(1, generator);
        T x(v.begin(), v.end());
        BOOST_CHECK(test::equivalent(x.hash_function(), hf));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq));
        BOOST_CHECK(test::equivalent(x.get_allocator(), al));
        test::check_container(x, v);
        test::check_equivalent_keys(x);
    }

    std::cerr<<"Construct 9\n";
    {
        T x(0, hf, eq, al);
        BOOST_CHECK(x.empty());
        BOOST_CHECK(test::equivalent(x.hash_function(), hf));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq));
        BOOST_CHECK(test::equivalent(x.get_allocator(), al));
        test::check_equivalent_keys(x);
    }

    std::cerr<<"Construct 10\n";
    {
        test::random_values<T> v(1000, generator);
        T x(v.begin(), v.end(), 10000, hf, eq, al);
        BOOST_CHECK(x.bucket_count() >= 10000);
        BOOST_CHECK(test::equivalent(x.hash_function(), hf));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq));
        BOOST_CHECK(test::equivalent(x.get_allocator(), al));
        test::check_container(x, v);
        test::check_equivalent_keys(x);
    }

    std::cerr<<"Construct 11\n";
    {
        test::random_values<T> v(1000, generator);
        T x(al);
        BOOST_CHECK(x.empty());
        BOOST_CHECK(test::equivalent(x.hash_function(), hf));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq));
        BOOST_CHECK(test::equivalent(x.get_allocator(), al));
        test::check_equivalent_keys(x);
    }
}

template <class T>
void constructor_tests2(T*, test::random_generator const& generator = test::default_generator)
{
    BOOST_DEDUCED_TYPENAME T::hasher hf;
    BOOST_DEDUCED_TYPENAME T::hasher hf1(1);
    BOOST_DEDUCED_TYPENAME T::hasher hf2(2);
    BOOST_DEDUCED_TYPENAME T::key_equal eq;
    BOOST_DEDUCED_TYPENAME T::key_equal eq1(1);
    BOOST_DEDUCED_TYPENAME T::key_equal eq2(2);
    BOOST_DEDUCED_TYPENAME T::allocator_type al;
    BOOST_DEDUCED_TYPENAME T::allocator_type al1(1);
    BOOST_DEDUCED_TYPENAME T::allocator_type al2(2);

    std::cerr<<"Construct 1\n";
    {
        T x(10000, hf1, eq1);
        BOOST_CHECK(x.bucket_count() >= 10000);
        BOOST_CHECK(test::equivalent(x.hash_function(), hf1));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq1));
        BOOST_CHECK(test::equivalent(x.get_allocator(), al));
        test::check_equivalent_keys(x);
    }

    std::cerr<<"Construct 2\n";
    {
        T x(100, hf1);
        BOOST_CHECK(x.empty());
        BOOST_CHECK(x.bucket_count() >= 100);
        BOOST_CHECK(test::equivalent(x.hash_function(), hf1));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq));
        BOOST_CHECK(test::equivalent(x.get_allocator(), al));
        test::check_equivalent_keys(x);
    }

    std::cerr<<"Construct 3\n";
    {
        test::random_values<T> v(100, generator);
        T x(v.begin(), v.end(), 0, hf1, eq1);
        BOOST_CHECK(test::equivalent(x.hash_function(), hf1));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq1));
        BOOST_CHECK(test::equivalent(x.get_allocator(), al));
        test::check_container(x, v);
        test::check_equivalent_keys(x);
    }

    std::cerr<<"Construct 4\n";
    {
        test::random_values<T> v(5, generator);
        T x(v.begin(), v.end(), 1000, hf1);
        BOOST_CHECK(x.bucket_count() >= 1000);
        BOOST_CHECK(test::equivalent(x.hash_function(), hf1));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq));
        BOOST_CHECK(test::equivalent(x.get_allocator(), al));
        test::check_container(x, v);
        test::check_equivalent_keys(x);
    }


    std::cerr<<"Construct 5\n";
    {
        test::random_values<T> v(100, generator);
        T x(v.begin(), v.end(), 0, hf, eq, al1);
        T y(x.begin(), x.end(), 0, hf1, eq1, al2);
        test::check_container(x, v);
        test::check_container(y, x);
        test::check_equivalent_keys(x);
        test::check_equivalent_keys(y);
    }

    std::cerr<<"Construct 6\n";
    {
        test::random_values<T> v(100, generator);
        T x(v.begin(), v.end(), 0, hf1, eq1);
        T y(x.begin(), x.end(), 0, hf, eq);
        test::check_container(x, v);
        test::check_container(y, x);
        test::check_equivalent_keys(x);
        test::check_equivalent_keys(y);
    }

    std::cerr<<"Construct 7\n";
    {
        test::random_values<T> v(100, generator);
        T x(v.begin(), v.end(), 0, hf1, eq1);
        T y(x.begin(), x.end(), 0, hf2, eq2);
        test::check_container(x, v);
        test::check_container(y, x);
        test::check_equivalent_keys(x);
        test::check_equivalent_keys(y);
    }

    std::cerr<<"Construct 8 - from input iterator\n";
    {
        test::random_values<T> v(100, generator);
        T x(test::input_iterator(v.begin()), test::input_iterator(v.end()), 0, hf1, eq1);
        T y(test::input_iterator(x.begin()), test::input_iterator(x.end()), 0, hf2, eq2);
        test::check_container(x, v);
        test::check_container(y, x);
        test::check_equivalent_keys(x);
        test::check_equivalent_keys(y);
    }
}

template <class T>
void map_constructor_test(T* = 0, test::random_generator const& generator = test::default_generator)
{
    std::cerr<<"map_constructor_test\n";

    typedef test::list<std::pair<BOOST_DEDUCED_TYPENAME T::key_type, BOOST_DEDUCED_TYPENAME T::mapped_type> > list;
    test::random_values<T> v(1000, generator);
    list l(v.begin(), v.end());
    T x(l.begin(), l.end());

    test::check_container(x, v);
    test::check_equivalent_keys(x);
}

boost::unordered_set<test::object, test::hash, test::equal_to, test::allocator<test::object> >* test_set;
boost::unordered_multiset<test::object, test::hash, test::equal_to, test::allocator<test::object> >* test_multiset;
boost::unordered_map<test::object, test::object, test::hash, test::equal_to, test::allocator<test::object> >* test_map;
boost::unordered_multimap<test::object, test::object, test::hash, test::equal_to, test::allocator<test::object> >* test_multimap;

using test::default_generator;
using test::generate_collisions;

UNORDERED_TEST(constructor_tests1,
    ((test_set)(test_multiset)(test_map)(test_multimap))
    ((default_generator)(generate_collisions))
)

UNORDERED_TEST(constructor_tests2,
    ((test_set)(test_multiset)(test_map)(test_multimap))
    ((default_generator)(generate_collisions))
)

UNORDERED_TEST(map_constructor_test,
    ((test_map)(test_multimap))
)

#if !defined(BOOST_NO_INITIALIZER_LISTS)

UNORDERED_AUTO_TEST(test_initializer_list) {
    std::cerr<<"Initializer List Tests\n";
    boost::unordered_set<int> x1 = { 2, 10, 45, -5 };
    BOOST_CHECK(x1.find(10) != x1.end());
    BOOST_CHECK(x1.find(46) == x1.end());
}

#endif

}

RUN_TESTS()
