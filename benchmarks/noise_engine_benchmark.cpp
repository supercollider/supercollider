#include <boost/cstdint.hpp>
#include <boost/random.hpp>

#include "benchmark_helpers.hpp"

#include "../source/dsp/noise_engines.hpp"

using namespace boost;
using namespace std;

using namespace nova::detail;

uint32_t dummy;

void __noinline__ bench_1(int n)
{
    uint32_t ret;
    cheap_noise_engine e;
    for (int i = 0; i != n; ++i)
        ret = e();
    dummy = ret;
}

void __noinline__ bench_2(int n)
{
    uint32_t ret;
    taus88_engine e;
    for (int i = 0; i != n; ++i)
        ret = e();
    dummy = ret;
}

void __noinline__ bench_3(int n)
{
    uint32_t ret;
    boost::taus88 e;
    //cout << "max: " << e.max() << endl;
    for (int i = 0; i != n; ++i)
        ret = e();
    dummy = ret;
}

void __noinline__ bench_4(int n)
{
    uint32_t ret;
    boost::minstd_rand e;
    //cout << "max: " << e.max() << endl;
    for (int i = 0; i != n; ++i)
        ret = e() * 2;
    dummy = ret;
}

void __noinline__ bench_5(int n)
{
    uint32_t ret;
    boost::rand48 e;
    //cout << "max: " << e.max() << endl;
    for (int i = 0; i != n; ++i)
        ret = e() * 2;
    dummy = ret;
}

int main()
{
    cout << "hardcoded linear congruental" << endl;
    run_bench(boost::bind(bench_1, 64));

    cout << endl << "taus88" << endl;
    run_bench(boost::bind(bench_2, 64));

    cout << endl << "boost taus88" << endl;
    run_bench(boost::bind(bench_3, 64));

    cout << endl << "boost minstd_rand" << endl;
    run_bench(boost::bind(bench_4, 64));

    cout << endl << "boost rand48" << endl;
    run_bench(boost::bind(bench_5, 64));
}
