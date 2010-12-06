#include <iostream>
#include <boost/test/unit_test.hpp>


#include "../source/dsp/sample_extractor.hpp"

const int size = 1000;

/* workaround for gcc bug on x86_64 */
namespace nova
{
inline float * get_samples(boost::array<float, size> & arg)
{
    return arg.begin();
}

inline const float * get_samples(boost::array<float, size> const & arg)
{
    return arg.begin();
}

inline double * get_samples(boost::array<double, size> & arg)
{
    return arg.begin();
}

inline const double * get_samples(boost::array<double, size> const & arg)
{
    return arg.begin();
}
}


#include "../source/dsp/xfade.hpp"

using namespace nova;
using namespace std;



namespace
{

template<typename sample_type>
void run_tests(void)
{
    boost::array<sample_type, size> in0;
    boost::array<sample_type, size> in1;

    boost::array<sample_type, size> out;

    {
        xfade<sample_type> xfade;
        xfade.init(-1);

        xfade.set_fade(1, size);

        xfade.perform(in0, in1, out, size);
    }

    {
        xfade_ar<sample_type> xfade2;
        boost::array<sample_type, size> fade;
        fade.assign(0);

        xfade2.perform(in0, in1, fade, out, size);
    }
}

} /* namespace */


BOOST_AUTO_TEST_CASE( xfade_float )
{
    run_tests<float>();
}

BOOST_AUTO_TEST_CASE( xfade_double )
{
    run_tests<double>();
}
