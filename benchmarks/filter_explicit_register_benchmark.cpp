#include "../source/utilities/high_resolution_timer.hpp"

#include "../source/dsp/filter.hpp"
#include <iostream>

#include <boost/array.hpp>


float f;

using namespace std;

void __attribute__ ((noinline)) loop(nova::biquad<float, float, false, true> & f, float * ip, float * op, int n)
{
    f.perform(ip, op, n);
}

void __attribute__ ((noinline)) loop(nova::biquad<float, float, false, false> & f, float * ip, float * op, int n)
{
    f.perform(ip, op, n);
}

template <bool copy_temporary>
void run_bench(void)
{
    boost::high_resolution_timer hrt;
    hrt.restart();

    static typename nova::biquad<float, float, false, copy_temporary> filter;

    static boost::array<float, 64> in, out;
    in.assign(0);

    float * ip = in.begin();
    float * op = out.begin();

    for (int i = 0; i != 4000000; ++i)
        loop(filter, ip, op, 64);

    double e = hrt.elapsed();

    std::cout << e << std::endl;
}

int main()
{
    using namespace std;

    cout << "copy temporary" << endl;
    run_bench<true>();

    cout << endl << "don't copy temporary" << endl;
    run_bench<false>();
}
