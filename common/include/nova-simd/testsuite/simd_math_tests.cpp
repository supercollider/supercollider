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


#define COMPARE_TEST(name, low, high)                                   \
template <typename float_type>                                          \
void test_##name(void)                                                  \
{                                                                       \
    aligned_array<float, size> ALIGNED sseval, mpval, libmval, args;    \
                                                                        \
    float init = low;                                                   \
    for (int i = 0; i != size; ++i)                                     \
    {                                                                   \
        args[i] = init;                                                 \
        init += (high - low)/size;                                      \
    }                                                                   \
                                                                        \
    name##_vec(libmval.begin(), args.begin(), size);                    \
    name##_vec_simd(sseval.begin(), args.begin(), size);                \
    name##_vec_simd<size>(mpval.begin(), args.begin());                 \
                                                                        \
    compare_buffers(sseval.begin(), libmval.begin(), 1e-4f);            \
    compare_buffers(mpval.begin(), libmval.begin(), 1e-4f);             \
}                                                                       \
                                                                        \
BOOST_AUTO_TEST_CASE( name##_tests)                                     \
{                                                                       \
    test_##name<float>();                                               \
    test_##name<double>();                                              \
}

COMPARE_TEST(sin, -1.5, 1.5)
COMPARE_TEST(cos, -1.5, 1.5)
COMPARE_TEST(tan, -1.5, 1.5)
COMPARE_TEST(tanh, -1.5, 1.5)
COMPARE_TEST(ssqrt, 0, 20)


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
