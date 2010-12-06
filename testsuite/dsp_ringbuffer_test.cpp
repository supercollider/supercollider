#include "../source/dsp/buffer.hpp"
#include <boost/test/unit_test.hpp>

#include <iostream>
using namespace std;

using namespace nova;

BOOST_AUTO_TEST_CASE( dsp_ringbuffer_test )
{
    /* single sample */
    {
        ring_buffer rb(5);

        sample in = 0.;

        for (int i = 0; i != 100; ++i)
        {
            sample out;
            rb.read_sampleblock(&out, 2, 1);

            if (i >= 2)
                BOOST_REQUIRE_EQUAL(out, in - 2.);

            rb.write_sampleblock(&in, 1);
            in += 1.0;
        }
    }

    /* multiple samples */
    {
        ring_buffer rb(5);

        sample in[2] = {0., 1.};

        for (int i = 0; i != 100; ++i)
        {
            sample out[2];
            rb.read_sampleblock(out, 3, 2);

            if (i >= 3)
            {
                BOOST_REQUIRE_EQUAL(out[0], in[0] - 3.);
                BOOST_REQUIRE_EQUAL(out[1], in[1] - 3.);
            }

            rb.write_sampleblock(in, 2);
            in[0] += 2.0;
            in[1] += 2.0;
        }
    }

}
