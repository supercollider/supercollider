#include <iostream>
#include <boost/test/unit_test.hpp>

#include "../source/dsp/fast_sinosc.hpp"
#include "../source/dsp/sample_extractor.hpp"
#include <cmath>

using namespace nova;
using namespace std;

BOOST_AUTO_TEST_CASE( fast_sinosc_test )
{
    boost::array<float, 1<<16> buffer;

    fast_sinosc<float> osc;
    osc.set_frequency(0.01);

    /* step response */
    for (int i = 0; i != 1<<16; i+=64)
    {
        float * out = buffer.begin()+i;
        osc.perform(out, 64);
    }
}
