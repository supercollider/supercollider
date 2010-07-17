#include <iostream>
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <cmath>

#include "../vec.hpp"

using namespace nova;
using namespace std;

template <typename F>
void test_get_set(void)
{
    vec<F> v;
    for (int i = 0; i != v.size; ++i)
        v.set(i, (F)1.0*i);

    for (int i = 0; i != v.size; ++i)
        BOOST_REQUIRE_EQUAL( v.get(i), F(1.0*i));

    vec<F> w;
    w.set_slope(0, 1.0);
    for (int i = 0; i != v.size; ++i)
        BOOST_REQUIRE_EQUAL( w.get(i), F(1.0*i));
}


BOOST_AUTO_TEST_CASE( get_set )
{
    test_get_set<float>();
    test_get_set<double>();
}
