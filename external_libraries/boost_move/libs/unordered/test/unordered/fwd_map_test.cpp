
// Copyright 2008-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/unordered/unordered_map_fwd.hpp>

typedef boost::unordered_map<int, int> int_map;

void call_swap(int_map& x, int_map& y) {
    swap(x,y);
}

bool call_equals(int_map& x, int_map& y) {
    return x == y;
}

bool call_not_equals(int_map& x, int_map& y) {
    return x != y;
}

typedef boost::unordered_multimap<int, int> int_multimap;

void call_swap(int_multimap& x, int_multimap& y) {
    swap(x,y);
}

bool call_equals(int_multimap& x, int_multimap& y) {
    return x == y;
}

bool call_not_equals(int_multimap& x, int_multimap& y) {
    return x != y;
}

#include <boost/unordered_map.hpp>
#include "../helpers/test.hpp"

UNORDERED_AUTO_TEST(use_map_fwd_declared_function) {
    int_map x, y;
    x[1] = 2;
    y[2] = 1;
    call_swap(x, y);

    BOOST_CHECK(y.find(1) != y.end() && y.find(1)->second == 2);
    BOOST_CHECK(y.find(2) == y.end());

    BOOST_CHECK(x.find(1) == x.end());
    BOOST_CHECK(x.find(2) != x.end() && x.find(2)->second == 1);

    BOOST_CHECK(!call_equals(x, y));
    BOOST_CHECK(call_not_equals(x, y));
}

UNORDERED_AUTO_TEST(use_multimap_fwd_declared_function) {
    int_multimap x, y;
    call_swap(x, y);
    BOOST_CHECK(call_equals(x, y));
    BOOST_CHECK(!call_not_equals(x, y));
}

RUN_TESTS()
