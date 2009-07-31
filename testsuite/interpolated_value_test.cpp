#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "../source/dsp/interpolated_value.hpp"

using namespace nova;
using namespace boost;

namespace
{

template <typename float_t>
void run_test(void)
{
    typedef  interpolated_value<float_t> iv_t;
    iv_t iv(0.1, 64);

    typedef typename iv_t::interpolator interpolator_t;

    {
        interpolator_t interpolator = iv.get_interpolator(1.1);

        for (int i = 0; i != 64; ++i)
            interpolator.step();
        BOOST_CHECK_CLOSE( interpolator.value, float_t(1.1), float_t(1e-20) );
    }

    BOOST_CHECK_CLOSE( iv.get_value(), float_t(1.1), float_t(1e-30) );
};

template <typename float_t>
void run_test_2(void)
{
    typedef  interpolated_value_long_interpolation<float_t> iv_t;
    iv_t iv(0.1, 1024);         /* multiple of 64 */

    typedef typename iv_t::interpolator interpolator_t;

    {
        iv.set_target(1.1);

        for (int i = 0; i != 16; ++i)
        {
            interpolator_t interpolator = iv.get_interpolator(64);

            for (int j = 0; j != 64; ++j)
                interpolator.step();
        }
    }

    BOOST_CHECK_CLOSE( iv.get_value(), float_t(1.1), float_t(1e-30) );
};

template <typename float_t>
void run_test_3(void)
{
    typedef  interpolated_value_long_interpolation<float_t> iv_t;
    iv_t iv(0.1, 999);         /* no multiple of 64 */

    typedef typename iv_t::interpolator interpolator_t;

    iv.set_target(1.1);

    for (int i = 0; i != 16; ++i)
    {
        const int steps_per_loop = 64;

        int steps = iv.get_remaining(steps_per_loop);

        {
            interpolator_t interpolator = iv.get_interpolator(steps);

            for (int j = 0; j != steps; ++j)
                interpolator.step();
        }

        int rest = steps_per_loop - steps;
        if (rest)
        {
            for (int j = 0; j != rest; ++j)
                iv.get_value();
        }
    }

    BOOST_CHECK_CLOSE( iv.get_value(), float_t(1.1), float_t(1e-4) );
};

}

BOOST_AUTO_TEST_CASE( interpolated_value_test )
{
    run_test<float>();
    run_test<double>();
}

BOOST_AUTO_TEST_CASE( interpolated_value_long_interpolation_test_1 )
{
    run_test_2<float>();
    run_test_2<double>();
}

BOOST_AUTO_TEST_CASE( interpolated_value_long_interpolation_test_2 )
{
    run_test_3<float>();
    run_test_3<double>();
}
