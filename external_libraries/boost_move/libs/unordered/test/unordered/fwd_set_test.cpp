
// Copyright 2008-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/unordered/unordered_set_fwd.hpp>

struct true_type { char x[100]; };
struct false_type { char x; };

false_type is_unordered_set_impl(void*);

template <class Value, class Hash, class Pred, class Alloc>
true_type is_unordered_set_impl(
        boost::unordered_set<Value, Hash, Pred, Alloc>*);

typedef boost::unordered_set<int> int_set;

void call_swap(int_set& x, int_set& y) {
    swap(x,y);
}

bool call_equals(int_set& x, int_set& y) {
    return x == y;
}

bool call_not_equals(int_set& x, int_set& y) {
    return x != y;
}

typedef boost::unordered_multiset<int> int_multiset;

void call_swap(int_multiset& x, int_multiset& y) {
    swap(x,y);
}

bool call_equals(int_multiset& x, int_multiset& y) {
    return x == y;
}

bool call_not_equals(int_multiset& x, int_multiset& y) {
    return x != y;
}

#include "../helpers/test.hpp"

UNORDERED_AUTO_TEST(use_fwd_declared_trait_without_definition) {
    BOOST_CHECK(sizeof(is_unordered_set_impl((int_set*) 0)) == sizeof(true_type));
}

#include <boost/unordered_set.hpp>

UNORDERED_AUTO_TEST(use_fwd_declared_trait) {
    boost::unordered_set<int> x;
    BOOST_CHECK(sizeof(is_unordered_set_impl(&x)) == sizeof(true_type));

    int dummy;
    BOOST_CHECK(sizeof(is_unordered_set_impl(&dummy)) == sizeof(false_type));
}

UNORDERED_AUTO_TEST(use_set_fwd_declared_function) {
    int_set x, y;
    x.insert(1);
    y.insert(2);
    call_swap(x, y);

    BOOST_CHECK(y.find(1) != y.end());
    BOOST_CHECK(y.find(2) == y.end());

    BOOST_CHECK(x.find(1) == x.end());
    BOOST_CHECK(x.find(2) != x.end());

    BOOST_CHECK(!call_equals(x, y));
    BOOST_CHECK(call_not_equals(x, y));
}

UNORDERED_AUTO_TEST(use_multiset_fwd_declared_function) {
    int_multiset x, y;
    call_swap(x, y);
    BOOST_CHECK(call_equals(x, y));
    BOOST_CHECK(!call_not_equals(x, y));
}

RUN_TESTS()
