#include <iostream>
#include <boost/test/unit_test.hpp>

#include "../source/dsp/muladd_helpers.hpp"
#include "../source/dsp/sample_extractor.hpp"
#include <cmath>

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


using namespace nova::detail;
using namespace std;

namespace
{

template<typename sample_type, typename muladd_type>
void run_test(boost::array<sample_type, size> & buffer, muladd_type & ma)
{
    for(unsigned int i = 0; i != buffer.size(); ++i)
        buffer[i] = ma(buffer[i]);
}


template<typename sample_type>
void run_tests(void)
{
    typedef boost::array<sample_type, size> sample_buffer;

    sample_buffer buffer;
    buffer.assign(1);

    {
        muladd_helper_nop<sample_type> ma;
        run_test(buffer, ma);
    }

    {
        muladd_helper_mul_c<sample_type> ma(1);
        run_test(buffer, ma);
    }

    {
        muladd_helper_mul_l<sample_type> ma(1, 0);
        run_test(buffer, ma);
    }

    {
        muladd_helper_mul_v<sample_type, sample_buffer> ma(buffer);
        run_test(buffer, ma);
    }



    {
        muladd_helper_add_c<sample_type> ma(0);
        run_test(buffer, ma);
    }

    {
        muladd_helper_add_l<sample_type> ma(0, 0);
        run_test(buffer, ma);
    }

    {
        muladd_helper_add_v<sample_type, sample_buffer> ma(buffer);
        run_test(buffer, ma);
    }



    {
        muladd_helper_mul_c_add_c<sample_type> ma(1, 0);
        run_test(buffer, ma);
    }

    {
        muladd_helper_mul_c_add_l<sample_type> ma(1, 0, 0);
        run_test(buffer, ma);
    }

    {
        muladd_helper_mul_c_add_v<sample_type, sample_buffer> ma(1, buffer);
        run_test(buffer, ma);
    }


    {
        muladd_helper_mul_l_add_c<sample_type> ma(1, 0, 0);
        run_test(buffer, ma);
    }

    {
        muladd_helper_mul_l_add_l<sample_type> ma(1, 0, 0, 0);
        run_test(buffer, ma);
    }

    {
        muladd_helper_mul_l_add_v<sample_type, sample_buffer> ma(1, 0, buffer);
        run_test(buffer, ma);
    }


    {
        muladd_helper_mul_v_add_c<sample_type, sample_buffer> ma(buffer, 0);
        run_test(buffer, ma);
    }

    {
        muladd_helper_mul_v_add_l<sample_type, sample_buffer> ma(buffer, 0, 0);
        run_test(buffer, ma);
    }

    {
        muladd_helper_mul_v_add_v<sample_type, sample_buffer> ma(buffer, buffer);
        run_test(buffer, ma);
    }
}

}

BOOST_AUTO_TEST_CASE( muladd_helpers_float )
{
    run_tests<float>();
}

BOOST_AUTO_TEST_CASE( muladd_helpers_double )
{
    run_tests<double>();
}
