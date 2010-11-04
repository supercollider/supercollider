#include <iostream>
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <cmath>

#include "../simd_mix.hpp"
#include "test_helper.hpp"
#include "../benchmarks/cache_aligned_array.hpp"

using namespace nova;
using namespace std;

static const int size = 512;

template <typename float_type>
void test_mix(void)
{
    aligned_array<float_type, size>  sseval, mpval, generic, args0, args1;
    randomize_buffer<float_type>(args0.c_array(), size);
    randomize_buffer<float_type>(args1.c_array(), size);

    float_type factor0 = 0.4;
    float_type factor1 = 0.6;

    mix_vec(generic.c_array(), args0.c_array(), factor0, args1.c_array(), factor1, size);
    mix_vec_simd(sseval.c_array(), args0.c_array(), factor0, args1.c_array(), factor1, size);
    mix_vec_simd<size>(mpval.c_array(), args0.c_array(), factor0, args1.c_array(), factor1);

    for (int i = 0; i != size; ++i) {
        BOOST_CHECK_CLOSE( sseval[i], generic[i], 0.0001 );
        BOOST_CHECK_CLOSE( mpval[i], generic[i], 0.0001 );
    }
}

BOOST_AUTO_TEST_CASE( mix_tests )
{
    test_mix<float>();
    test_mix<double>();
}

template <typename float_type>
void test_mix_ramp(void)
{
    aligned_array<float_type, size>  sseval, mpval, generic, args0, args1;
    randomize_buffer<float_type>(args0.c_array(), size);
    randomize_buffer<float_type>(args1.c_array(), size);

    float_type factor0 = 0.4; float_type slope0 = 0.1/size;
    float_type factor1 = 0.6; float_type slope1 = -0.1/size;

    mix_vec(generic.c_array(), args0.c_array(), factor0, slope0, args1.c_array(), factor1, slope1, size);
    mix_vec_simd(sseval.c_array(), args0.c_array(), factor0, slope0, args1.c_array(), factor1, slope1, size);
    mix_vec_simd<size>(mpval.c_array(), args0.c_array(), factor0, slope0, args1.c_array(), factor1, slope1);

    for (int i = 0; i != size; ++i) {
        BOOST_CHECK_CLOSE( sseval[i], generic[i], 0.5 );
        BOOST_CHECK_CLOSE( mpval[i], generic[i], 0.5 );
    }
}

BOOST_AUTO_TEST_CASE( mix_ramp_tests )
{
    test_mix_ramp<float>();
    test_mix_ramp<double>();
}
