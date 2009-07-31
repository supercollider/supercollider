#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "../source/dsp/env_follower.hpp"
#include "../source/dsp/sample_extractor.hpp"

using namespace nova;
using namespace std;

BOOST_AUTO_TEST_CASE( env_follower_test )
{
    env_follower<float> env;

    env.set_coeffs(5000, 5000);

    /* step response */
    for (int i = 0; i != 10000; ++i)
    {
        float in = 1.f;
        float * ip = &in;

        env.perform(ip, 1);
    }

    BOOST_CHECK_SMALL( env.current_envelope() - float(0.999), float(1e-3) );

    for (int i = 0; i != 10000; ++i)
    {
        float in = 0.f;
        float * ip = &in;

        env.perform(ip, 1);
    }

    BOOST_CHECK_SMALL( env.current_envelope(), float(1e-3) );
}
