#include <boost/test/unit_test.hpp>

#include "../source/dsp/median_noise.hpp"



using namespace nova;
using namespace std;


const int size = 1024;

namespace
{

template <typename generator>
void run_test(generator & gen)
{
    boost::array<float, size> out;

    gen.set_median(4);
    float * begin = out.begin();
    gen.perform(begin, size);

    gen.set_median(8);
    gen.perform(begin, size);
}

template <typename generator, typename engine>
void run_test(generator & gen, engine & e)
{
    boost::array<float, size> out;

    float * begin = out.begin();
    gen.set_median(4);
    gen.perform(begin, size, e);

    gen.set_median(8);
    gen.perform(begin, size, e);
}

detail::cheap_noise_engine engine;

}

BOOST_AUTO_TEST_CASE( white_median_test_1 )
{
    median_white_noise<float> mwn;
    run_test(mwn);
}

BOOST_AUTO_TEST_CASE( white_median_test_2 )
{
    median_white_noise_external_engine<float, detail::cheap_noise_engine> mwn;
    run_test(mwn, engine);
}


BOOST_AUTO_TEST_CASE( white_median_test_3 )
{
    lf0_median_white_noise<float> mwn;
    mwn.set_frequency(0.1);
    run_test(mwn);
    mwn.set_frequency(0.2);
    run_test(mwn);

    mwn.set_frequency(1000, 44100);
    run_test(mwn);
    mwn.set_frequency(2000, 44100);
    run_test(mwn);
}

BOOST_AUTO_TEST_CASE( white_median_test_4 )
{
    lf0_median_white_noise_external_engine<float, detail::cheap_noise_engine> mwn;

    mwn.set_frequency(0.1);
    run_test(mwn, engine);
    mwn.set_frequency(0.2);
    run_test(mwn, engine);

    mwn.set_frequency(1000, 44100);
    run_test(mwn, engine);
    mwn.set_frequency(2000, 44100);
    run_test(mwn, engine);
}
