#include <boost/test/unit_test.hpp>

#include <list>
#include <vector>
#include <set>
#include <iostream>

#include "utilities/tl_allocator.hpp"

using namespace nova;
using namespace std;

namespace {

struct initialized_struct {
    initialized_struct(int i): i(i), f(0.2) {}

    bool operator<(initialized_struct const& rhs) const { return i < rhs.i; }

    int i;
    double f;
};

template <typename T> void test_list(void) {
    list<T, tl_allocator<T>> vec;

    for (int i = 0; i != 8192; ++i)
        vec.push_back(T(i));
}


template <typename T> void test_vector(void) {
    vector<T, tl_allocator<T>> vec;

    for (int i = 0; i != 8192; ++i)
        vec.push_back(T(i));
}

template <typename T> void test_set(void) {
    set<T, std::less<T>, tl_allocator<T>> set;

    for (int i = 0; i != 8192; ++i)
        set.insert(T(i));
}

}

BOOST_AUTO_TEST_CASE(rt_alloc_test_1) { detail::tl_allocator<8 * 1024 * 1024> rt_alloc; }

BOOST_AUTO_TEST_CASE(rt_alloc_test_2) {
    test_list<int>();
    test_list<initialized_struct>();
}

BOOST_AUTO_TEST_CASE(rt_alloc_test_3) {
    test_vector<int>();
    test_vector<initialized_struct>();
}

BOOST_AUTO_TEST_CASE(rt_alloc_test_4) {
    test_set<int>();
    test_set<initialized_struct>();
}
