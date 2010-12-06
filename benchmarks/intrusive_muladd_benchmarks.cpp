#include <cmath>
#include <iostream>

#include <boost/function.hpp>

#include "benchmark_helpers.hpp"

#include "../source/dsp/cache_aligned_array.hpp"
#include "../source/dsp/sample_extractor.hpp"
#include "../source/dsp/fast_sinosc.hpp"
#include "../source/dsp/simd_sse.hpp"
#include "../source/dsp/simd_static.hpp"

using namespace nova;

fast_sinosc<float> osc;
aligned_array<float, 64> buffer;
aligned_array<float, 64> ramp;
aligned_array<float, 64> ramp2;


namespace nova
{
inline float * get_samples(nova::aligned_array<float, 64> & arg)
{
    return arg.begin();
}

inline const float * get_samples(nova::aligned_array<float, 64> const & arg)
{
    return arg.begin();
}
}


void __attribute__ ((noinline)) bench_nop(unsigned int n)
{
    osc.perform(buffer, n);
}



void __attribute__ ((noinline)) bench_mul_c(unsigned int n)
{
    detail::muladd_helper_mul_c<float> ma(0.5);

    osc.perform(buffer, n, ma);
}

void __attribute__ ((noinline)) bench_mul_c_external(unsigned int n)
{
    osc.perform(buffer, n);
    times_vec_simd(buffer.begin(), buffer.begin(), 0.5f, n);
}

void __attribute__ ((noinline)) bench_mul_c_external_mp(unsigned int n)
{
    osc.perform(buffer, n);
    times_vec_simd<64>(buffer.begin(), buffer.begin(), 0.5);
}


void __attribute__ ((noinline)) bench_mul_l(unsigned int n)
{
    detail::muladd_helper_mul_l<float> ma(0, 0.01);

    osc.perform(buffer, n, ma);
}

void __attribute__ ((noinline)) bench_mul_l_external(unsigned int n)
{
    osc.perform(buffer, n);

    float factor = 0;
    for (unsigned int i = 0; i != n; ++i)
    {
        buffer[i] *= factor;
        factor += 0.01;
    }
}


void __attribute__ ((noinline)) bench_mul_v(unsigned int n)
{
    detail::muladd_helper_mul_v<float> ma(ramp.begin());

    osc.perform(buffer, n, ma);
}

void __attribute__ ((noinline)) bench_mul_v_external(unsigned int n)
{
    osc.perform(buffer, n);

    times_vec_simd(buffer.begin(), buffer.begin(), ramp.begin(), n);
}

void __attribute__ ((noinline)) bench_mul_v_external_mp(unsigned int n)
{
    osc.perform(buffer, n);

    times_vec_simd<64>(buffer.begin(), buffer.begin(), ramp.begin());
}





void __attribute__ ((noinline)) bench_add_c(unsigned int n)
{
    detail::muladd_helper_add_c<float> ma(0.5);

    osc.perform(buffer, n, ma);
}

void __attribute__ ((noinline)) bench_add_c_external(unsigned int n)
{
    osc.perform(buffer, n);
    plus_vec_simd(buffer.begin(), buffer.begin(), 0.5f, n);
}

void __attribute__ ((noinline)) bench_add_c_external_mp(unsigned int n)
{
    osc.perform(buffer, n);
    plus_vec_simd<64>(buffer.begin(), buffer.begin(), 0.5);
}


void __attribute__ ((noinline)) bench_add_l(unsigned int n)
{
    detail::muladd_helper_add_l<float> ma(0, 0.01);

    osc.perform(buffer, n, ma);
}

void __attribute__ ((noinline)) bench_add_l_external(unsigned int n)
{
    osc.perform(buffer, n);

    float factor = 0;
    for (unsigned int i = 0; i != n; ++i)
    {
        buffer[i] *= factor;
        factor += 0.01;
    }
}


void __attribute__ ((noinline)) bench_add_v(unsigned int n)
{
    detail::muladd_helper_add_v<float> ma(ramp.begin());

    osc.perform(buffer, n, ma);
}

void __attribute__ ((noinline)) bench_add_v_external(unsigned int n)
{
    osc.perform(buffer, n);

    plus_vec_simd(buffer.begin(), buffer.begin(), ramp.begin(), n);
}

void __attribute__ ((noinline)) bench_add_v_external_mp(unsigned int n)
{
    osc.perform(buffer, n);

    plus_vec_simd<64>(buffer.begin(), buffer.begin(), ramp.begin());
}




void __attribute__ ((noinline)) bench_mul_c_add_c(unsigned int n)
{
    detail::muladd_helper_mul_c_add_c<float> ma(0.5, 0.5);

    osc.perform(buffer, n, ma);
}

void __attribute__ ((noinline)) bench_mul_c_add_c_external(unsigned int n)
{
    osc.perform(buffer, n);
    muladd_vec_simd(buffer.begin(), buffer.begin(), 0.5, 0.5, n);
}

void __attribute__ ((noinline)) bench_mul_c_add_c_external_mp(unsigned int n)
{
    osc.perform(buffer, n);
    muladd_vec_simd<64>(buffer.begin(), buffer.begin(), 0.5, 0.5);
}


void __attribute__ ((noinline)) bench_mul_c_add_l(unsigned int n)
{
    detail::muladd_helper_mul_c_add_l<float> ma(0.5, 0, 0.01);

    osc.perform(buffer, n, ma);
}

void __attribute__ ((noinline)) bench_mul_c_add_l_external(unsigned int n)
{
    osc.perform(buffer, n);

    float offset = 0;
    for (unsigned int i = 0; i != n; ++i)
    {
        buffer[i] = buffer[i] * 0.5 + offset;
        offset += 0.01;
    }
}

void __attribute__ ((noinline)) bench_mul_c_add_v(unsigned int n)
{
    detail::muladd_helper_mul_c_add_v<float> ma(0.5, ramp.begin());

    osc.perform(buffer, n, ma);
}

void __attribute__ ((noinline)) bench_mul_c_add_v_external(unsigned int n)
{
    osc.perform(buffer, n);
    muladd_vec_simd(buffer.begin(), buffer.begin(), 0.5, ramp.begin(), n);
}

void __attribute__ ((noinline)) bench_mul_c_add_v_external_mp(unsigned int n)
{
    osc.perform(buffer, n);
    muladd_vec_simd<64>(buffer.begin(), buffer.begin(), 0.5, ramp.begin());
}




void __attribute__ ((noinline)) bench_mul_l_add_c(unsigned int n)
{
    detail::muladd_helper_mul_l_add_c<float> ma(0, 0.01, 0.5);

    osc.perform(buffer, n, ma);
}

void __attribute__ ((noinline)) bench_mul_l_add_c_external(unsigned int n)
{
    osc.perform(buffer, n);
    float factor = 0;
    for (unsigned int i = 0; i != n; ++i)
    {
        buffer[i] = buffer[i] * factor + 0.5f;
        factor += 0.01f;
    }
}

void __attribute__ ((noinline)) bench_mul_l_add_l(unsigned int n)
{
    detail::muladd_helper_mul_l_add_l<float> ma(1, 0.02, 0, 0.01);

    osc.perform(buffer, n, ma);
}

void __attribute__ ((noinline)) bench_mul_l_add_l_external(unsigned int n)
{
    osc.perform(buffer, n);

    float offset = 0;
    float factor = 1;
    for (unsigned int i = 0; i != n; ++i)
    {
        buffer[i] = buffer[i] * factor + offset;
        offset += 0.01f;
        factor += 0.02f;
    }
}

void __attribute__ ((noinline)) bench_mul_l_add_v(unsigned int n)
{
    detail::muladd_helper_mul_l_add_v<float> ma(0, 0.02, ramp.begin());

    osc.perform(buffer, n, ma);
}

void __attribute__ ((noinline)) bench_mul_l_add_v_external(unsigned int n)
{
    osc.perform(buffer, n);

    float factor = 1;
    for (unsigned int i = 0; i != n; ++i)
    {
        buffer[i] = buffer[i] * factor + ramp[i];
        factor += 0.02f;
    }
}



void __attribute__ ((noinline)) bench_mul_v_add_c(unsigned int n)
{
    detail::muladd_helper_mul_v_add_c<float> ma(ramp.begin(), 0.5);

    osc.perform(buffer, n, ma);
}

void __attribute__ ((noinline)) bench_mul_v_add_c_external(unsigned int n)
{
    osc.perform(buffer, n);
    muladd_vec_simd(buffer.begin(), buffer.begin(), ramp.begin(), 0.5, n);
}

void __attribute__ ((noinline)) bench_mul_v_add_c_external_mp(unsigned int n)
{
    osc.perform(buffer, n);
    muladd_vec_simd<64>(buffer.begin(), buffer.begin(), ramp.begin(), 0.5);
}


void __attribute__ ((noinline)) bench_mul_v_add_l(unsigned int n)
{
    detail::muladd_helper_mul_v_add_l<float> ma(ramp.begin(), 0, 0.01);

    osc.perform(buffer, n, ma);
}

void __attribute__ ((noinline)) bench_mul_v_add_l_external(unsigned int n)
{
    osc.perform(buffer, n);

    float offset = 0;
    for (unsigned int i = 0; i != n; ++i)
    {
        buffer[i] = buffer[i] * ramp[i] + offset;
        offset += 0.01;
    }
}

void __attribute__ ((noinline)) bench_mul_v_add_v(unsigned int n)
{
    detail::muladd_helper_mul_v_add_v<float> ma(ramp.begin(), ramp2.begin());

    osc.perform(buffer, n, ma);
}

void __attribute__ ((noinline)) bench_mul_v_add_v_external(unsigned int n)
{
    osc.perform(buffer, n);
    muladd_vec_simd(buffer.begin(), buffer.begin(), ramp.begin(), ramp2.begin(), n);
}

void __attribute__ ((noinline)) bench_mul_v_add_v_external_mp(unsigned int n)
{
    osc.perform(buffer, n);
    muladd_vec_simd<64>(buffer.begin(), buffer.begin(), ramp.begin(), ramp2.begin());
}

int main()
{
    using namespace std;

    osc.set_frequency(440 / 44100);

    buffer.assign(0);

    for (int i = 0; i != 64; ++i)
        ramp[i] = ramp2[i] = i * 0.01;

    cout << "no muladd" << endl;
    run_bench(boost::bind(bench_nop, 64));


    cout << endl << endl << "constant multiplication, intrusive" << endl;
    run_bench(boost::bind(bench_mul_c, 64));

    cout << "constant multiplication, external (SIMD)" << endl;
    run_bench(boost::bind(bench_mul_c_external, 64));

    cout << "constant multiplication, external (SIMD, unrolled)" << endl;
    run_bench(boost::bind(bench_mul_c_external_mp, 64));


    cout << endl << "line multiplication, intrusive"<< endl;
    run_bench(boost::bind(bench_mul_l, 64));

    cout << "line multiplication, external"<< endl;
    run_bench(boost::bind(bench_mul_l_external, 64));


    cout << endl << "vector multiplication, intrusive" << endl;
    run_bench(boost::bind(bench_mul_v, 64));

    cout << "vector multiplication, external (SIMD)" << endl;
    run_bench(boost::bind(bench_mul_v_external, 64));

    cout << "vector multiplication, external (SIMD, unrolled)" << endl;
    run_bench(boost::bind(bench_mul_v_external_mp, 64));




    cout << endl << endl << "constant addition, intrusive" << endl;
    run_bench(boost::bind(bench_add_c, 64));

    cout << "constant addition, external (SIMD)" << endl;
    run_bench(boost::bind(bench_add_c_external, 64));

    cout << "constant addition, external (SIMD, unrolled)" << endl;
    run_bench(boost::bind(bench_add_c_external_mp, 64));

    cout << endl << "line addition, intrusive"<< endl;
    run_bench(boost::bind(bench_add_l, 64));

    cout << "line addition, external"<< endl;
    run_bench(boost::bind(bench_add_l_external, 64));

    cout << endl << "vector addition, intrusive" << endl;
    run_bench(boost::bind(bench_add_v, 64));

    cout << "vector addition, external (SIMD)" << endl;
    run_bench(boost::bind(bench_add_v_external, 64));

    cout << "vector addition, external (SIMD, unrolled)" << endl;
    run_bench(boost::bind(bench_add_v_external_mp, 64));



    cout << endl << endl << "constant multiplication, constant addition, intrusive" << endl;
    run_bench(boost::bind(bench_mul_c_add_c, 64));

    cout << "constant multiplication, constant addition, external (SIMD)" << endl;
    run_bench(boost::bind(bench_mul_c_add_c_external, 64));

    cout << "constant multiplication, constant addition, external (SIMD, unrolled)" << endl;
    run_bench(boost::bind(bench_mul_c_add_c_external_mp, 64));


    cout << endl << "constant multiplication, line addition, intrusive" << endl;
    run_bench(boost::bind(bench_mul_c_add_l, 64));

    cout << "constant multiplication, line addition" << endl;
    run_bench(boost::bind(bench_mul_c_add_l_external, 64));


    cout << endl << "constant multiplication, vector addition, intrusive" << endl;
    run_bench(boost::bind(bench_mul_c_add_v, 64));

    cout << "constant multiplication, vector addition, external (SIMD)" << endl;
    run_bench(boost::bind(bench_mul_c_add_v_external, 64));

    cout << "constant multiplication, vector addition, external (SIMD, unrolled)" << endl;
    run_bench(boost::bind(bench_mul_c_add_v_external_mp, 64));


    cout << endl << "line multiplication, constant addition, intrusive"<< endl;
    run_bench(boost::bind(bench_mul_l_add_c, 64));

    cout << "line multiplication, constant addition, external"<< endl;
    run_bench(boost::bind(bench_mul_l_add_c_external, 64));


    cout << endl << "line multiplication, line addition, intrusive"<< endl;
    run_bench(boost::bind(bench_mul_l_add_l, 64));

    cout << "line multiplication, line addition, external"<< endl;
    run_bench(boost::bind(bench_mul_l_add_l_external, 64));


    cout << endl << "line multiplication, vector addition, intrusive"<< endl;
    run_bench(boost::bind(bench_mul_l_add_v, 64));

    cout << "line multiplication, vector addition, external"<< endl;
    run_bench(boost::bind(bench_mul_l_add_v_external, 64));


    cout << endl << endl << "vector multiplication, constant addition, intrusive" << endl;
    run_bench(boost::bind(bench_mul_v_add_c, 64));

    cout << "vector multiplication, constant addition, external (SIMD)" << endl;
    run_bench(boost::bind(bench_mul_v_add_c_external, 64));

    cout << "vector multiplication, constant addition, external (SIMD, unrolled)" << endl;
    run_bench(boost::bind(bench_mul_v_add_c_external_mp, 64));


    cout << endl << "vector multiplication, line addition, intrusive" << endl;
    run_bench(boost::bind(bench_mul_v_add_l, 64));

    cout << "vector multiplication, line addition" << endl;
    run_bench(boost::bind(bench_mul_v_add_l_external, 64));


    cout << endl << "vector multiplication, vector addition, intrusive" << endl;
    run_bench(boost::bind(bench_mul_v_add_v, 64));

    cout << "vector multiplication, vector addition, external (SIMD)" << endl;
    run_bench(boost::bind(bench_mul_v_add_v_external, 64));

    cout << "vector multiplication, vector addition, external (SIMD, unrolled)" << endl;
    run_bench(boost::bind(bench_mul_v_add_v_external_mp, 64));
}
