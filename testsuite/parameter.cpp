#include <iostream>
#include <boost/test/unit_test.hpp>

#include "../source/dsp/parameter.hpp"

using namespace std;

BOOST_AUTO_TEST_CASE( noninterpolating_parameter_test )
{
    nova::detail::noninterpolating_parameter<float> par(0);

    BOOST_REQUIRE (par == 0);

    par = 1.0;

    for (int i = 0; i != 10; ++i)
        BOOST_REQUIRE (par == 1);

    nova::detail::noninterpolating_parameter<float> par2 (par);
    nova::detail::noninterpolating_parameter<float> par3;

    par3 = par;
    BOOST_REQUIRE (par == par2);
    BOOST_REQUIRE (par2 == par3);
    BOOST_REQUIRE (par == par3);

}

BOOST_AUTO_TEST_CASE( exponential_interpolating_parameter_test )
{
    nova::detail::exponential_interpolating_parameter<float> par(0);

    BOOST_REQUIRE (par == 0);

    par = 1.0;

    for (int i = 0; i != 10; ++i)
    {
        float out = par;

        BOOST_REQUIRE (out != 1);
    }

    nova::detail::exponential_interpolating_parameter<float> par2 (par);
    nova::detail::exponential_interpolating_parameter<float> par3;

    par3 = par;

    for (int i = 0; i != 10; ++i)
    {
        float f = par;
        float f2 = par2;
        float f3 = par3;

        BOOST_REQUIRE (f == f2);
        BOOST_REQUIRE (f2 == f3);
    }
}

BOOST_AUTO_TEST_CASE( linear_interpolating_parameter_test )
{
    nova::detail::linear_interpolating_parameter<float> par(0);

    BOOST_REQUIRE (par == 0);

    par = 1.0;

    for (int i = 0; i != 10; ++i)
    {
        float out = par;

        BOOST_REQUIRE (out != 1);
    }

    nova::detail::linear_interpolating_parameter<float> par2 (par);
    nova::detail::linear_interpolating_parameter<float> par3;

    par3 = par;

    for (int i = 0; i != 100; ++i)
    {
        float f = par;
        float f2 = par2;
        float f3 = par3;

        BOOST_REQUIRE (f == f2);
        BOOST_REQUIRE (f2 == f3);
    }
}
