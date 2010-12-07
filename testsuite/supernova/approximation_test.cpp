#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <cmath>

#include "utilities/fast_approximations.hpp"

using namespace nova;
using namespace std;

namespace
{

template <typename F>
void run_fast_atan2_tests(void)
{
    for (F y = -4; y < 4; y+=0.01)
        for (F x = -4; x < 4; x+=0.01)
        {
            if (abs(x) < 1e-8)
                continue;

            F accurate = atan2(y, x);
            F fast = fast_atan2(y, x);
            F faster = faster_atan2(y, x);

            BOOST_CHECK_SMALL( accurate - fast,   F(0.011) );
            BOOST_CHECK_SMALL( accurate - faster, F(0.072) );
        }
}

}

BOOST_AUTO_TEST_CASE( atan2_tests_float )
{
    run_fast_atan2_tests<float>();
}

BOOST_AUTO_TEST_CASE( atan2_tests_double )
{
    run_fast_atan2_tests<double>();
}
