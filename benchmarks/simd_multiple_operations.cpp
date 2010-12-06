#include "benchmark_helpers.hpp"
#include "../source/dsp/cache_aligned_array.hpp"
#include "../source/dsp/fast_sinosc.hpp"
#include "../source/dsp/simd_sse.hpp"
#include "../source/dsp/simd_static.hpp"

using namespace nova;
using namespace std;

aligned_array<float, 64> out0, out1;
aligned_array<float, 64> in0,  in1;

void __noinline__ bench_separate(unsigned int n)
{
    times_vec_ramp_simd(out0.begin(), in0.begin(), 0, 0.01, n);
    times_vec_ramp_simd(out1.begin(), in1.begin(), 1, -0.01, n);
}

void __noinline__ bench_separate_2(unsigned int n)
{
    times_vec_simd(out0.begin(), in0.begin(), 0.4f, n);
    times_vec_simd(out1.begin(), in1.begin(), 0.6f, n);
}

void __noinline__ bench_combined(unsigned int n)
{
    times_vec2_ramp_simd(out0.begin(), in0.begin(), 0, 0.01,
                         out1.begin(), in1.begin(), 1, -0.01, n);
}

void __noinline__ bench_combined_2(unsigned int n)
{
    times_vec2_simd(out0.begin(), in0.begin(), 0.4,
                    out1.begin(), in1.begin(), 0.6, n);
}

int main(void)
{
    out0.assign(0);
    out1.assign(0);
    in0.assign(0);
    in1.assign(0);

    const unsigned int iterations = 50000000;

    cout << "combined:" << endl;
    run_bench(boost::bind(bench_combined, 64), iterations);
    run_bench(boost::bind(bench_combined_2, 64), iterations);

    cout << "\nseparate:" << endl;
    run_bench(boost::bind(bench_separate, 64), iterations);
    run_bench(boost::bind(bench_separate_2, 64), iterations);
}
