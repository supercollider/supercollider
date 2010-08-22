
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

#include <iostream>

namespace assign_tests {

test::seed_t seed(96785);

template <class T>
void assign_tests1(T*, test::random_generator generator = test::default_generator)
{
    BOOST_DEDUCED_TYPENAME T::hasher hf;
    BOOST_DEDUCED_TYPENAME T::key_equal eq;

    std::cerr<<"assign_tests1.1\n";
    {
        T x;
        x = x;
        BOOST_CHECK(x.empty());
        BOOST_CHECK(test::equivalent(x.hash_function(), hf));
        BOOST_CHECK(test::equivalent(x.key_eq(), eq));
    }

    std::cerr<<"assign_tests1.2\n";
    {
        test::random_values<T> v(1000, generator);
        T x(v.begin(), v.end());

        test::ordered<T> tracker = test::create_ordered(x);
        tracker.insert_range(v.begin(), v.end());

        x = x;
        tracker.compare(x);

        T y;
        y.max_load_factor(x.max_load_factor() / 20);
        y = x;
        tracker.compare(y);
        BOOST_CHECK(x.max_load_factor() == y.max_load_factor());
    }
}

template <class T>
void assign_tests2(T*, test::random_generator generator = test::default_generator)
{
    BOOST_DEDUCED_TYPENAME T::hasher hf1(1);
    BOOST_DEDUCED_TYPENAME T::hasher hf2(2);
    BOOST_DEDUCED_TYPENAME T::key_equal eq1(1);
    BOOST_DEDUCED_TYPENAME T::key_equal eq2(2);
    BOOST_DEDUCED_TYPENAME T::allocator_type al1(1);
    BOOST_DEDUCED_TYPENAME T::allocator_type al2(2);

    std::cerr<<"assign_tests2.1\n";
    {
        test::random_values<T> v(1000, generator);
        T x1(v.begin(), v.end(), 0, hf1, eq1);
        T x2(0, hf2, eq2);
        x2 = x1;
        BOOST_CHECK(test::equivalent(x2.hash_function(), hf1));
        BOOST_CHECK(test::equivalent(x2.key_eq(), eq1));
        test::check_container(x2, v);
    }

    std::cerr<<"assign_tests2.2\n";
    {
        test::random_values<T> v1(100, generator), v2(100, generator);
        T x1(v1.begin(), v1.end(), 0, hf1, eq1, al1);
        T x2(v2.begin(), v2.end(), 0, hf2, eq2, al2);
        x2 = x1;
        BOOST_CHECK(test::equivalent(x2.hash_function(), hf1));
        BOOST_CHECK(test::equivalent(x2.key_eq(), eq1));
        BOOST_CHECK(test::equivalent(x2.get_allocator(), al2));
        test::check_container(x2, v1);
    }
}

boost::unordered_set<test::object, test::hash, test::equal_to, test::allocator<test::object> >* test_set;
boost::unordered_multiset<test::object, test::hash, test::equal_to, test::allocator<test::object> >* test_multiset;
boost::unordered_map<test::object, test::object, test::hash, test::equal_to, test::allocator<test::object> >* test_map;
boost::unordered_multimap<test::object, test::object, test::hash, test::equal_to, test::allocator<test::object> >* test_multimap;

using test::default_generator;
using test::generate_collisions;

UNORDERED_TEST(assign_tests1,
    ((test_set)(test_multiset)(test_map)(test_multimap))
    ((default_generator)(generate_collisions))
)

UNORDERED_TEST(assign_tests2,
    ((test_set)(test_multiset)(test_map)(test_multimap))
    ((default_generator)(generate_collisions))
)

#if !defined(BOOST_NO_INITIALIZER_LISTS)

UNORDERED_AUTO_TEST(assign_initializer_list)
{
    std::cerr<<"Initializer List Tests\n";

    boost::unordered_set<int> x;
    x.insert(10);
    x.insert(20);
    x = { 1, 2, -10 };
    BOOST_CHECK(x.find(10) == x.end());
    BOOST_CHECK(x.find(-10) != x.end());
}

#endif

}

RUN_TESTS()
