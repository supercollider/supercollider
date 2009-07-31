#include <boost/cstdint.hpp>

#include "../source/utilities/high_resolution_timer.hpp"
#include <cmath>
#include <limits>
#include <iostream>
#include <cassert>

extern "C" float __attribute__ ((noinline)) bench_sc_style(unsigned int rand)
{
    union
    {
        boost::uint32_t i;
        float f;
    } u;
    u.i = 0x40000000 | (rand >> 9);
    return u.f - 3.f;
}

extern "C" float __attribute__ ((noinline)) bench_pd_style(unsigned int rand)
{
    int val = rand;
    return ((float)((val & 0x7fffffff) - 0x40000000)) *
        (float)(1.0 / 0x40000000);
}

extern "C" float __attribute__ ((noinline)) bench_generic(unsigned int rand)
{
    return 2.f * float(rand) / (float(std::numeric_limits<int>::max()) + 1.f) - 1.f;
}

extern "C" double __attribute__ ((noinline)) bench_generic_double(unsigned int rand)
{
    return 2.0 * double(rand) / (double(std::numeric_limits<int>::max()) + 1.0) - 1.0;
}



float f;
template <float (*bench)(unsigned int)>
void run_bench(void)
{
    boost::high_resolution_timer hrt;
    hrt.restart();

    for (unsigned int i = 0; i != 1000000000; ++i)
    {
        f = (*bench)(i);
/*         assert((f <= 1) and (f >= -1)); */
    }
    double e = hrt.elapsed();

    std::cout << e << std::endl;
}

double d;
template <double (*bench)(unsigned int)>
void run_bench(void)
{
    boost::high_resolution_timer hrt;
    hrt.restart();

    for (unsigned int i = 0; i != 1000000000; ++i)
    {
        d = (*bench)(i);
/*         assert((d <= 1) and (d >= -1)); */
    }
    double e = hrt.elapsed();

    std::cout << e << std::endl;
}

int main()
{
    using namespace std;

    cout << "sc-style" << endl;
    run_bench<bench_sc_style>();

    cout << endl << "pd-style" << endl;
    run_bench<bench_pd_style>();

    cout << endl << "generic" << endl;
    run_bench<bench_generic>();

    cout << endl << "generic double" << endl;
    run_bench<bench_generic_double>();
}
