#include "benchmark_helpers.hpp"
#include "../simd_unary_arithmetic.hpp"

#define sc_abs(a)  ((a)>=0?(a) : -(a))

nova::aligned_array<float, 1024> in;
nova::aligned_array<float, 1024> out;


void __noinline__ bench_1(unsigned int numSamples)
{
    for (unsigned int i = 0; i != numSamples; ++i)
        out[i] = sc_abs(in[i]);
}

void __noinline__ bench_2(unsigned int numSamples)
{
    for (unsigned int i = 0; i != numSamples; ++i)
        out[i] = std::abs(in[i]);
}

void __noinline__ bench_3(unsigned int numSamples)
{
    for (unsigned int i = 0; i != numSamples; i += 8)
    {
        out[i] = std::abs(in[i]);
        out[i+1] = std::abs(in[i+1]);
        out[i+2] = std::abs(in[i+2]);
        out[i+3] = std::abs(in[i+3]);
        out[i+4] = std::abs(in[i+4]);
        out[i+5] = std::abs(in[i+5]);
        out[i+6] = std::abs(in[i+6]);
        out[i+7] = std::abs(in[i+7]);
    }
}

using namespace nova;

void __noinline__ bench_4(unsigned int numSamples)
{
    abs_vec_simd<64>(out.begin(), in.begin());
}

void __noinline__ bench_5(unsigned int numSamples)
{
    abs_vec_simd(out.begin(), in.begin(), numSamples);
}


int main(void)
{
    /* touch buffers */
    out.assign(0);
    fill_container(in);

    const unsigned int iterations = 50000000;

    run_bench(boost::bind(bench_1, 64), iterations);
    run_bench(boost::bind(bench_2, 64), iterations);
    run_bench(boost::bind(bench_3, 64), iterations);
    run_bench(boost::bind(bench_4, 64), iterations);
    run_bench(boost::bind(bench_5, 64), iterations);
}
