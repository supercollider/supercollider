#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

#include "../source/dsp/lag.hpp"
#include "../source/dsp/sample_extractor.hpp"

#include "../libs/gnuplot-cpp/gnuplot_i.hpp"

#include <cmath>
#include <vector>

using namespace nova;
using namespace std;

template <int order>
void test_lag(void)
{
    lag_n<order, float> lag;
    lag.set_lag(0.8, 1000);

    float current = 1;

    std::vector<float> out(1000, 0);

    for (int i = 0; i != 1000; ++i)
        lag.tick(current, out[i]);

    string label ("lag");
    label += boost::lexical_cast<string>(order);


#if 0
    /* hm ... may have issues on multiprocessor machines? */
    Gnuplot g1 = Gnuplot(label);

    g1.savetops(string("testsuite/test_") + label);

    g1.plot_x(out, label);
#endif
}

BOOST_AUTO_TEST_CASE( lag_test )
{
    test_lag<1>();
    test_lag<2>();
    test_lag<3>();
    test_lag<4>();
}
