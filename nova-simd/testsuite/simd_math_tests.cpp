#include <iostream>
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <cmath>

#include "../simd_math.hpp"
#include "../benchmarks/cache_aligned_array.hpp"

using namespace nova;
using namespace std;


static const int size = 10000;

/* test range: -1.5, 1.5 */
BOOST_AUTO_TEST_CASE( tan_tests_float )
{
    aligned_array<float, size> sseval, libmval, args;

    float init = -1.5;
    for (int i = 0; i != size; ++i)
    {
        args[i] = init;
        init += 3.f/size;
    }

    for (int i = 0; i != size; ++i)
        libmval[i] = tan(args[i]);

    for (int i = 0; i != size; i+=4)
        tan4(&sseval[i], &args[i]);

    for (int i = 0; i != size; ++i)
        BOOST_CHECK_CLOSE( sseval[i], libmval[i], 0.0001 );
}


/* test range: 0, 20 */
BOOST_AUTO_TEST_CASE( pow_tests_float_1 )
{
    for (float exponent = 0.1; exponent < 2; exponent += 0.1)
    {
        aligned_array<float, size> sseval, libmval, args;

        float init = 0;
        for (int i = 0; i != size; ++i)
        {
            args[i] = init;
            init += 20.f/size;
        }

        for (int i = 0; i != size; ++i)
            libmval[i] = pow(args[i], exponent);

        for (int i = 0; i != size; i+=4)
            pow4(&sseval[i], &args[i], exponent);

        for (int i = 0; i != size; ++i)
            BOOST_REQUIRE_CLOSE( sseval[i], libmval[i], 0.0001 );
    }
}

namespace {

float spow(float arg, float e)
{
    if (arg > 0)
        return std::pow(arg, e);
    else
        return -std::pow(-arg, e);
}

float ssqrt(float arg)
{
    if (arg > 0)
        return std::sqrt(arg);
    else
        return -std::sqrt(-arg);
}

} /* namespace */

/* test range: -10, 10 */
BOOST_AUTO_TEST_CASE( spow_tests_float_1 )
{
    for (float exponent = 0.1; exponent < 2; exponent += 0.1)
    {
        aligned_array<float, size> sseval, libmval, args;

        aligned_array<float, 4> exparray;
        exparray.assign(exponent);

        float init = -10;
        for (int i = 0; i != size; ++i)
        {
            args[i] = init;
            init += 20.f/size;
        }

        for (int i = 0; i != size; ++i)
            libmval[i] = spow(args[i], exponent);

        for (int i = 0; i != size; i+=4)
            spow4(&sseval[i], &args[i], exparray.begin());

        for (int i = 0; i != size; ++i)
            BOOST_REQUIRE_CLOSE( sseval[i], libmval[i], 0.0001 );
    }
}

/* test range: 0, 20 */
BOOST_AUTO_TEST_CASE( sqrt_tests_float_1 )
{
    aligned_array<float, size> sseval, libmval, args;

    float init = 0;
    for (int i = 0; i != size; ++i)
    {
        args[i] = init;
        init += 20.f/size;
    }

    for (int i = 0; i != size; ++i)
        libmval[i] = ssqrt(args[i]);

    for (int i = 0; i != size; i+=4)
        ssqrt4(&sseval[i], &args[i]);

    for (int i = 0; i != size; ++i)
        BOOST_REQUIRE_CLOSE( sseval[i], libmval[i], 0.0001 );
}


/* test range: -2.5, 2.5 */
BOOST_AUTO_TEST_CASE( tanh_tests_float )
{
    aligned_array<float, size> sseval, libmval, args;

    float init = -2.5;
    for (int i = 0; i != size; ++i)
    {
        args[i] = init;
        init += 5.f/size;
    }

    for (int i = 0; i != size; ++i)
        libmval[i] = tanh(args[i]);

    for (int i = 0; i != size; i+=4)
        tanh4(&sseval[i], &args[i]);

    for (int i = 0; i != size; ++i)
        BOOST_CHECK_CLOSE( sseval[i], libmval[i], 0.0001 );
}

/* test range: -25, 25 */
BOOST_AUTO_TEST_CASE( tanh_tests_float_2 )
{
    aligned_array<float, size> sseval, libmval, args;

    float init = -25;
    for (int i = 0; i != size; ++i)
    {
        args[i] = init;
        init += 50.f/size;
    }

    for (int i = 0; i != size; ++i)
        libmval[i] = tanh(args[i]);

    for (int i = 0; i != size; i+=4)
        tanh4(&sseval[i], &args[i]);

    for (int i = 0; i != size; ++i)
        BOOST_CHECK_CLOSE( sseval[i], libmval[i], 0.0001 );
}

#ifdef __GNUC__
namespace {
inline void fabsf4(float * out, const float * in)
{
    const vec_float4 * ip = (const vec_float4*)in;
    vec_float4 * op = (vec_float4*)out;
    *op = _fabsf4(*ip);
}
}

/* test range: -1.5, 1.5 */
BOOST_AUTO_TEST_CASE( fabs_tests_float )
{
    aligned_array<float, size> sseval, libmval, args;

    float init = -1.5;
    for (int i = 0; i != size; ++i)
    {
        args[i] = init;
        init += 3.f/size;
    }

    for (int i = 0; i != size; ++i)
        libmval[i] = abs(args[i]);

    for (int i = 0; i != size; i+=4)
        fabsf4(&sseval[i], &args[i]);

    for (int i = 0; i != size; ++i)
        BOOST_CHECK_CLOSE( sseval[i], libmval[i], 0.0001 );
}

#endif
