#include <boost/test/unit_test.hpp>

#include "../source/dsp/interpolation.hpp"

#include <cmath>
#include <iostream>

using namespace nova;
using namespace std;


bool float_equal(float lhs, float rhs)
{
    return abs(rhs - lhs) < 1e-7;
}


BOOST_AUTO_TEST_CASE( interpolation_test )
{
    float buf[] = {-1, 1, 0, -1.5, -2, 3, 6};

    for (int i = 0; i != 100; ++i)
    {
        float index = float(i) * 0.01;
        BOOST_REQUIRE( (float_equal(interpolate<1>(buf, index), -1) ) );

/*         cerr << interpolate<2>(buf, index) << " " << linearinterp(index, -1., 1.) << " " << linearinterp(index, -1., 1.) - interpolate<2>(buf, index) << endl; */

        BOOST_REQUIRE( (float_equal(interpolate<2>(buf, index),
                                    linearinterp(index, -1., 1.)
                            ) ) );
/*         BOOST_CHECK( (float_equal(interpolate<4>(buf, 1 + index), */
/*                                   cubicinterp(index, -1.0, 1.0, 0., -1.5) */
/*                           ) ) ); */


/*         float a = interpolate(buf, 1 + index, 4); */
/*         float b = cubicinterp(index, -1.0, 1.0, 0., -1.5); */

/*         std::cout << a << " " << b << " " << a-b << std::endl; */

        BOOST_REQUIRE( (float_equal(interpolate(buf, index, 1), -1) ) );
        BOOST_REQUIRE( (float_equal(interpolate(buf, index, 2),
                                    linearinterp(index, -1., 1.)
                            ) ) );

/*         BOOST_CHECK( (float_equal(interpolate(buf, 1 + index, 4), */
/*                                   cubicinterp(index, -1.0, 1.0, 0., -1.5) */
/*                           ) ) ); */

    }
}
