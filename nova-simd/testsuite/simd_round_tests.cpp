#include <iostream>
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <cmath>

#include "../simd_round.hpp"
#include "test_helper.hpp"
#include "../benchmarks/cache_aligned_array.hpp"

using namespace nova;
using namespace std;

static const int size = 10000;

#define COMPARE_TEST(name)                                              \
template <typename float_type>                                          \
void test_##name(void)                                                  \
{                                                                       \
    aligned_array<float_type, size> ALIGNED sseval, mpval, generic, args; \
    randomize_buffer<float_type>(args.c_array(), size);                 \
                                                                        \
    name##_vec(generic.c_array(), args.c_array(), size);                \
    name##_vec_simd(sseval.c_array(), args.c_array(), size);            \
    name##_vec_simd<size>(mpval.c_array(), args.c_array());             \
                                                                        \
    for (int i = 0; i != size; ++i)                                     \
        BOOST_CHECK_CLOSE( sseval[i], generic[i], 0.0001 );             \
    for (int i = 0; i != size; ++i)                                     \
        BOOST_CHECK_CLOSE( mpval[i], generic[i], 0.0001 );              \
}

COMPARE_TEST(round)
COMPARE_TEST(ceil)
COMPARE_TEST(floor)
COMPARE_TEST(frac)

BOOST_AUTO_TEST_CASE( round_tests )
{
    test_round<float>();
    test_round<double>();
}

BOOST_AUTO_TEST_CASE( floor_tests )
{
    test_floor<float>();
    test_floor<double>();
}

BOOST_AUTO_TEST_CASE( ceil_tests )
{
    test_ceil<float>();
    test_ceil<double>();
}

BOOST_AUTO_TEST_CASE( frac_tests )
{
    test_frac<float>();
    test_frac<double>();
}
