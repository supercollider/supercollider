#include "benchmark_helpers.hpp"
#include "../source/dsp/cache_aligned_array.hpp"
#include "../source/dsp/fast_sinosc.hpp"
#include "../source/simd/simd_binary_arithmetic.hpp"

using namespace nova;
using namespace std;

aligned_array<float, 64> out0, out1;
aligned_array<float, 64> in0,  in1;

const int samples_per_loop = 16;

inline void times_vec_ramp_simd(float * dest, const float * src1, float src2, float slope, uint n)
{
    __m128 factor = _mm_set_ps(src2, src2 + slope, src2 + slope+slope, src2 + slope+slope+slope);
    __m128 offset = _mm_set_ps1((slope+slope)+(slope+slope));

    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src1;

    n /= 8;
    for (uint i = 0; i != n; ++i)
    {
        *out++ = _mm_mul_ps(*in++, factor);
        factor = _mm_add_ps(factor, offset);
        *out++ = _mm_mul_ps(*in++, factor);
        factor = _mm_add_ps(factor, offset);
    }
}

inline void times_vec2_ramp_simd(float * dest0, const float * src01, float src02, float slope0,
                                 float * dest1, const float * src11, float src12, float slope1,
                                 uint n)
{
    /* checked: faster than separate loops (at least on x86_86, core2 duo)
     * */

    __m128 factor0 = _mm_set_ps(src02, src02 + slope0, src02 + slope0+slope0, src02 + slope0+slope0+slope0);
    __m128 offset0 = _mm_set_ps1((slope0+slope0)+(slope0+slope0));

    __m128 factor1 = _mm_set_ps(src12, src12 + slope1, src12 + slope1+slope1, src12 + slope1+slope1+slope1);
    __m128 offset1 = _mm_set_ps1((slope1+slope1)+(slope1+slope1));

    __m128 *out0 = (__m128*) dest0;
    const __m128 *in0 = (__m128*) src01;

    __m128 *out1 = (__m128*) dest1;
    const __m128 *in1 = (__m128*) src11;

    /* check: which unrolling makes sense? 8 seems to be more efficient on x86_64/core2 duo */
    n /= 8;
    do
    {
        *out0++ = _mm_mul_ps(*in0++, factor0);
        factor0 = _mm_add_ps(factor0, offset0);

        *out1++ = _mm_mul_ps(*in1++, factor1);
        factor1 = _mm_add_ps(factor1, offset1);

        *out0++ = _mm_mul_ps(*in0++, factor0);
        factor0 = _mm_add_ps(factor0, offset0);

        *out1++ = _mm_mul_ps(*in1++, factor1);
        factor1 = _mm_add_ps(factor1, offset1);
    }
    while (--n);
}

inline void times_vec2_simd(float * dest0, const float * src0, float f0,
                            float * dest1, const float * src1, float f1,
                            uint n)
{
    /* checked: faster than separate loops
     * */

    n /= samples_per_loop;
    do
    {
        times_vec_simd_mp<samples_per_loop>(dest0, src0, f0);
        times_vec_simd_mp<samples_per_loop>(dest1, src1, f1);
        dest0 += samples_per_loop;
        src0 += samples_per_loop;
        dest1 += samples_per_loop;
        src1 += samples_per_loop;
    }
    while (--n);
}


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
