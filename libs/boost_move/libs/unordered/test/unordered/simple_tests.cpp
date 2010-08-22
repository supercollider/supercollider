
// Copyright 2006-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This test checks the runtime requirements of containers.

#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include "../helpers/test.hpp"
#include <cstdlib>
#include <algorithm>
#include "../helpers/equivalent.hpp"

template <class X>
void simple_test(X const& a)
{
    test::unordered_equivalence_tester<X> equivalent(a);

    {
        X u;
        BOOST_CHECK(u.size() == 0);
        BOOST_CHECK(X().size() == 0);
    }

    {
        BOOST_CHECK(equivalent(X(a)));
    }

    {
        X u(a);
        BOOST_CHECK(equivalent(u));
    }

    {
        X u = a;
        BOOST_CHECK(equivalent(u));
    }

    {
        X b(a);
        BOOST_CHECK(b.begin() == const_cast<X const&>(b).cbegin());
        BOOST_CHECK(b.end() == const_cast<X const&>(b).cend());
    }

    {
        X b(a);
        X c;
        BOOST_CHECK(equivalent(b));
        BOOST_CHECK(c.empty());
        b.swap(c);
        BOOST_CHECK(b.empty());
        BOOST_CHECK(equivalent(c));
        b.swap(c);
        BOOST_CHECK(c.empty());
        BOOST_CHECK(equivalent(b));
    }

    {
        X u;
        X& r = u;
        BOOST_CHECK(&(r = r) == &r);
        BOOST_CHECK(r.empty());
        BOOST_CHECK(&(r = a) == &r);
        BOOST_CHECK(equivalent(r));
        BOOST_CHECK(&(r = r) == &r);
        BOOST_CHECK(equivalent(r));
    }

    {
        BOOST_CHECK(a.size() ==
                (BOOST_DEDUCED_TYPENAME X::size_type) std::distance(a.begin(), a.end()));
    }

    {
        BOOST_CHECK(a.empty() == (a.size() == 0));
    }

    {
        BOOST_CHECK(a.empty() == (a.begin() == a.end()));
        X u;
        BOOST_CHECK(u.begin() == u.end());
    }
}

UNORDERED_AUTO_TEST(simple_tests)
{
    using namespace std;
    srand(14878);

    std::cout<<"Test unordered_set.\n";
    boost::unordered_set<int> set;
    simple_test(set);
    
    set.insert(1); set.insert(2); set.insert(1456);
    simple_test(set);

    std::cout<<"Test unordered_multiset.\n";
    boost::unordered_multiset<int> multiset;
    simple_test(multiset);
    
    for(int i1 = 0; i1 < 1000; ++i1) {
        int count = rand() % 10, index = rand();
        for(int j = 0; j < count; ++j)
            multiset.insert(index);
    }
    simple_test(multiset);
    
    std::cout<<"Test unordered_map.\n";
    boost::unordered_map<int, int> map;

    for(int i2 = 0; i2 < 1000; ++i2) {
        map.insert(std::pair<const int, int>(rand(), rand()));
    }
    simple_test(map);

    std::cout<<"Test unordered_multimap.\n";
    boost::unordered_multimap<int, int> multimap;

    for(int i3 = 0; i3 < 1000; ++i3) {
        int count = rand() % 10, index = rand();
        for(int j = 0; j < count; ++j)
            multimap.insert(std::pair<const int, int>(index, rand()));
    }
    simple_test(multimap);
}

RUN_TESTS()
