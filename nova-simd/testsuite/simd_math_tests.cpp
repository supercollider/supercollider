#include <iostream>
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <cmath>

#include "../simd_math.hpp"
#include "../benchmarks/cache_aligned_array.hpp"
#include "test_helper.hpp"

using namespace nova;
using namespace std;


static const int size = 10000;

/* test range: -1.5, 1.5 */
BOOST_AUTO_TEST_CASE( tan_tests_float )
{
    aligned_array<float, size> ALIGNED sseval, libmval, args;

    float init = -1.5;
    for (int i = 0; i != size; ++i)
    {
        args[i] = init;
        init += 3.f/size;
    }

    tan_vec(libmval.begin(), args.begin(), size);
    tan_vec_simd(sseval.begin(), args.begin(), size);

    compare_buffers(sseval.begin(), libmval.begin(), 1e-4f);
}


/* test range: 0, 20 */
BOOST_AUTO_TEST_CASE( pow_tests_float_1 )
{
    for (float exponent = 0.1; exponent < 2; exponent += 0.1)
    {
        aligned_array<float, size> ALIGNED sseval, libmval, args;

        float init = 0;
        for (int i = 0; i != size; ++i)
        {
            args[i] = init;
            init += 20.f/size;
        }

        pow_vec(libmval.begin(), args.begin(), exponent, size);
        pow_vec_simd(sseval.begin(), args.begin(), exponent, size);

        compare_buffers(sseval.begin(), libmval.begin(), 1e-4f);
    }
}


/* test range: -10, 10 */
BOOST_AUTO_TEST_CASE( spow_tests_float_1 )
{
    for (float exponent = 0.1; exponent < 2; exponent += 0.1)
    {
        aligned_array<float, size> ALIGNED sseval, libmval, args;

        aligned_array<float, 4> exparray;
        exparray.assign(exponent);

        float init = -10;
        for (int i = 0; i != size; ++i)
        {
            args[i] = init;
            init += 20.f/size;
        }

        spow_vec(libmval.begin(), args.begin(), exponent, size);
        spow_vec_simd(sseval.begin(), args.begin(), exponent, size);

        compare_buffers(sseval.begin(), libmval.begin(), 1e-4f);
    }
}

/* test range: 0, 20 */
BOOST_AUTO_TEST_CASE( sqrt_tests_float_1 )
{
    aligned_array<float, size> ALIGNED sseval, libmval, args;

    float init = 0;
    for (int i = 0; i != size; ++i)
    {
        args[i] = init;
        init += 20.f/size;
    }

    ssqrt_vec(libmval.begin(), args.begin(), size);
    ssqrt_vec_simd(sseval.begin(), args.begin(), size);

    compare_buffers(sseval.begin(), libmval.begin(), 1e-4f);
}


/* test range: -2.5, 2.5 */
BOOST_AUTO_TEST_CASE( tanh_tests_float )
{
    aligned_array<float, size> ALIGNED sseval, libmval, args;

    float init = -2.5;
    for (int i = 0; i != size; ++i)
    {
        args[i] = init;
        init += 5.f/size;
    }

    tanh_vec(libmval.begin(), args.begin(), size);
    tanh_vec_simd(sseval.begin(), args.begin(), size);

    compare_buffers(sseval.begin(), libmval.begin(), 1e-4f);
}

/* test range: -25, 25 */
BOOST_AUTO_TEST_CASE( tanh_tests_float_2 )
{
    aligned_array<float, size> ALIGNED sseval, libmval, args;

    float init = -25;
    for (int i = 0; i != size; ++i)
    {
        args[i] = init;
        init += 50.f/size;
    }

    tanh_vec(libmval.begin(), args.begin(), size);
    tanh_vec_simd(sseval.begin(), args.begin(), size);

    compare_buffers(sseval.begin(), libmval.begin(), 1e-4f);
}
