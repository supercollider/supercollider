#include <boost/test/unit_test.hpp>

#include <list>
#include <vector>
#include <set>
#include <iostream>

#include "utilities/static_allocator.hpp"

using namespace nova;
using namespace std;

namespace {

template <int size, bool b> void test_pool_instatiation(void) { static_allocator<int, size, b> alloc; }

struct initialized_struct {
    initialized_struct(int i): i(i), f(0.2) {}

    bool operator<(initialized_struct const& rhs) const { return i < rhs.i; }

    int i;
    double f;
};

template <typename T, bool b> void test_list(void) {
    list<T, static_allocator<T, 8192, b>> vec;

    for (int i = 0; i != 8192; ++i)
        vec.push_back(T(i));
}


template <typename T, bool b> void test_vector(void) {
    vector<T, static_allocator<T, 8192, b>> vec;

    for (int i = 0; i != 8192; ++i)
        vec.push_back(T(i));
}

template <typename T, bool b> void test_set(void) {
    set<T, std::less<T>, static_allocator<T, 8192, b>> set;

    for (int i = 0; i != 8192; ++i)
        set.insert(T(i));
}

}
BOOST_AUTO_TEST_CASE(static_alloc_test_1) {
    test_pool_instatiation<16, false>();
    test_pool_instatiation<128, false>();
    test_pool_instatiation<1024, false>();
    test_pool_instatiation<4096, false>();

    test_pool_instatiation<16, true>();
    test_pool_instatiation<128, true>();
    test_pool_instatiation<1024, true>();
    test_pool_instatiation<4096, true>();
}


#if 0
BOOST_AUTO_TEST_CASE( static_alloc_test_2 )
{
    test_list<int, false>();
    test_list<initialized_struct, false>();

    test_list<int, true>();
    test_list<initialized_struct, true>();
}
#endif

BOOST_AUTO_TEST_CASE(static_alloc_test_3) {
    test_vector<int, false>();
    test_vector<initialized_struct, false>();

    test_vector<int, true>();
    test_vector<initialized_struct, true>();
}

#if 0
BOOST_AUTO_TEST_CASE( static_alloc_test_4 )
{
    test_set<int, false>();
    test_set<initialized_struct, false>();

    test_set<int, true>();
    test_set<initialized_struct, true>();
}
#endif
