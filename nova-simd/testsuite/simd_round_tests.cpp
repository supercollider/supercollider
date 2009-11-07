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

template <typename float_type>
void test_round(void)
{
    aligned_array<float_type, size> sseval, generic, args;
    randomize_buffer<float_type>(args.c_array(), size);

    round_vec(generic.c_array(), args.c_array(), size);
    round_vec_simd(sseval.c_array(), args.c_array(), size);

    for (int i = 0; i != size; ++i)
        BOOST_CHECK_CLOSE( sseval[i], generic[i], 0.0001 );
}

template <typename float_type>
void test_ceil(void)
{
    aligned_array<float_type, size> sseval, generic, args;
    randomize_buffer<float_type>(args.c_array(), size);

    ceil_vec(generic.c_array(), args.c_array(), size);
    ceil_vec_simd(sseval.c_array(), args.c_array(), size);

    for (int i = 0; i != size; ++i)
        BOOST_CHECK_CLOSE( sseval[i], generic[i], 0.0001 );
}

template <typename float_type>
void test_floor(void)
{
    aligned_array<float_type, size> sseval, generic, args;
    randomize_buffer<float_type>(args.c_array(), size);

    floor_vec(generic.c_array(), args.c_array(), size);
    floor_vec_simd(sseval.c_array(), args.c_array(), size);

    for (int i = 0; i != size; ++i)
        BOOST_CHECK_CLOSE( sseval[i], generic[i], 0.0001 );
}

template <typename float_type>
void test_frac(void)
{
    aligned_array<float_type, size> sseval, generic, args;
    randomize_buffer<float_type>(args.c_array(), size);

    frac_vec(generic.c_array(), args.c_array(), size);
    frac_vec_simd(sseval.c_array(), args.c_array(), size);

    for (int i = 0; i != size; ++i)
        BOOST_CHECK_CLOSE( sseval[i], generic[i], 0.0001 );
}

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
