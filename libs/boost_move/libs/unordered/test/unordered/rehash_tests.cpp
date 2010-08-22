
// Copyright 2006-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include "../helpers/test.hpp"
#include "../helpers/random_values.hpp"
#include "../helpers/tracker.hpp"

namespace rehash_tests
{

test::seed_t seed(2974);

template <class X>
bool postcondition(X const& x, BOOST_DEDUCED_TYPENAME X::size_type n)
{
    return x.bucket_count() > x.size() / x.max_load_factor() && x.bucket_count() >= n;
}

template <class X>
void rehash_empty_test1(X* = 0)
{
    X x;

    x.rehash(10000);
    BOOST_CHECK(postcondition(x, 10000));

    x.rehash(0);
    BOOST_CHECK(postcondition(x, 0));
}

template <class X>
void rehash_test1(X* = 0, test::random_generator generator = test::default_generator)
{
    test::random_values<X> v(1000, generator);
    test::ordered<X> tracker;
    tracker.insert_range(v.begin(), v.end());
    X x(v.begin(), v.end());

    x.rehash(0); BOOST_CHECK(postcondition(x, 0));
    tracker.compare(x);

    x.max_load_factor(0.25);
    x.rehash(0); BOOST_CHECK(postcondition(x, 0));
    tracker.compare(x);

    x.max_load_factor(50.0);
    x.rehash(0); BOOST_CHECK(postcondition(x, 0));
    tracker.compare(x);

    x.rehash(1000); BOOST_CHECK(postcondition(x, 1000));
    tracker.compare(x);
}

boost::unordered_set<int>* int_set_ptr;
boost::unordered_multiset<int>* int_multiset_ptr;
boost::unordered_map<int, int>* int_map_ptr;
boost::unordered_multimap<int, int>* int_multimap_ptr;

UNORDERED_TEST(rehash_empty_test1,
    ((int_set_ptr)(int_multiset_ptr)(int_map_ptr)(int_multimap_ptr))
)
UNORDERED_TEST(rehash_test1,
    ((int_set_ptr)(int_multiset_ptr)(int_map_ptr)(int_multimap_ptr))
)

}

RUN_TESTS()
