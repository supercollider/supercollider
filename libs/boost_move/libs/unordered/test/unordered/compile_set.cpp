
// Copyright 2006-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This test creates the containers with members that meet their minimum
// requirements. Makes sure everything compiles and is defined correctly.

#include <boost/unordered_set.hpp>

#include <iostream>
#include "../helpers/test.hpp"
#include "../objects/minimal.hpp"
#include "./compile_tests.hpp"

// Explicit instantiation to catch compile-time errors

template class boost::unordered_set<
    test::minimal::assignable,
    test::minimal::hash<test::minimal::assignable>,
    test::minimal::equal_to<test::minimal::assignable>,
    test::minimal::allocator<test::minimal::assignable> >;
template class boost::unordered_multiset<
    test::minimal::assignable,
    test::minimal::hash<test::minimal::assignable>,
    test::minimal::equal_to<test::minimal::assignable>,
    test::minimal::allocator<test::minimal::assignable> >;

UNORDERED_AUTO_TEST(test0)
{
    test::minimal::assignable assignable = test::minimal::assignable::create();

    std::cout<<"Test unordered_set.\n";
    boost::unordered_set<int> int_set;
    boost::unordered_set<
        test::minimal::assignable,
        test::minimal::hash<test::minimal::assignable>,
        test::minimal::equal_to<test::minimal::assignable>,
        test::minimal::allocator<test::minimal::assignable> > set;

    container_test(int_set, 0);
    container_test(set, assignable);

    std::cout<<"Test unordered_multiset.\n";
    boost::unordered_multiset<int> int_multiset;
    boost::unordered_multiset<
        test::minimal::assignable,
        test::minimal::hash<test::minimal::assignable>,
        test::minimal::equal_to<test::minimal::assignable>,
        test::minimal::allocator<test::minimal::assignable> > multiset;

    container_test(int_multiset, 0);
    container_test(multiset, assignable);
}

UNORDERED_AUTO_TEST(equality_tests) {
    typedef test::minimal::assignable value_type;

    boost::unordered_set<int> int_set;

    boost::unordered_set<
        test::minimal::assignable,
        test::minimal::hash<test::minimal::assignable>,
        test::minimal::equal_to<test::minimal::assignable>,
        test::minimal::allocator<value_type> > set;

    equality_test(int_set);
    equality_test(set);

    boost::unordered_multiset<int> int_multiset;

    boost::unordered_multiset<
        test::minimal::assignable,
        test::minimal::hash<test::minimal::assignable>,
        test::minimal::equal_to<test::minimal::assignable>,
        test::minimal::allocator<value_type> > multiset;

    equality_test(int_multiset);
    equality_test(multiset);
}

UNORDERED_AUTO_TEST(test1)
{
    boost::hash<int> hash;
    std::equal_to<int> equal_to;
    int value = 0;

    std::cout<<"Test unordered_set.\n";

    boost::unordered_set<int> set;
    
    unordered_unique_test(set, value);
    unordered_set_test(set, value);
    unordered_test(set, value, value, hash, equal_to);

    std::cout<<"Test unordered_multiset.\n";

    boost::unordered_multiset<int> multiset;
    
    unordered_equivalent_test(multiset, value);
    unordered_set_test(multiset, value);
    unordered_test(multiset, value, value, hash, equal_to);
}

UNORDERED_AUTO_TEST(test2)
{
    test::minimal::assignable assignable
        = test::minimal::assignable::create();
    test::minimal::copy_constructible copy_constructible
        = test::minimal::copy_constructible::create();
    test::minimal::hash<test::minimal::assignable> hash
        = test::minimal::hash<test::minimal::assignable>::create();
    test::minimal::equal_to<test::minimal::assignable> equal_to
        = test::minimal::equal_to<test::minimal::assignable>::create();

    std::cout<<"Test unordered_set.\n";

    boost::unordered_set<
        test::minimal::assignable,
        test::minimal::hash<test::minimal::assignable>,
        test::minimal::equal_to<test::minimal::assignable>,
        test::minimal::allocator<test::minimal::assignable> > set;

    unordered_unique_test(set, assignable);
    unordered_set_test(set, assignable);
    unordered_test(set, assignable, assignable, hash, equal_to);

    std::cout<<"Test unordered_multiset.\n";

    boost::unordered_multiset<
        test::minimal::assignable,
        test::minimal::hash<test::minimal::assignable>,
        test::minimal::equal_to<test::minimal::assignable>,
        test::minimal::allocator<test::minimal::assignable> > multiset;

    unordered_equivalent_test(multiset, assignable);
    unordered_set_test(multiset, assignable);
    unordered_test(multiset, assignable, assignable, hash, equal_to);
}

RUN_TESTS()
