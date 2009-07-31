#include "benchmark_helpers.hpp"

#include <cmath>
#include <iostream>

void __noinline__ bench_1(float * out_sample, unsigned int n)
{
    float b1 = std::cos(0.01);
    float y1 = 0;
    float y2 = 1;

    while (n)
    {
        float y0 = b1 * y1 - y2;
        *out_sample++ = y0;
        --n;
    }
}

void __noinline__ bench_1a(float * out_sample, unsigned int n)
{
    float b1 = std::cos(0.01);
    float y1 = 0;
    float y2 = 1;

    --out_sample;
    while (n)
    {
        float y0 = b1 * y1 - y2;
        *(++out_sample) = y0;
        --n;
    }
}

void __noinline__ bench_2(float * out_sample, unsigned int n)
{
    float b1 = std::cos(0.01);
    float y1 = 0;
    float y2 = 1;

    float * end_sample = out_sample + n;

    while (out_sample != end_sample)
    {
        float y0 = b1 * y1 - y2;
        *out_sample++ = y0;
    }
}

void __noinline__ bench_3(float * out_sample, unsigned int n)
{
    float b1 = std::cos(0.01);
    float y1 = 0;
    float y2 = 1;

    for (int i = 0; i != n; ++i)
    {
        float y0 = b1 * y1 - y2;
        *out_sample++ = y0;
    }
}

void __noinline__ bench_3a(float * out_sample, unsigned int n)
{
    float b1 = std::cos(0.01);
    float y1 = 0;
    float y2 = 1;

    out_sample--;
    for (int i = 0; i != n; ++i)
    {
        float y0 = b1 * y1 - y2;
        *(++out_sample) = y0;
    }
}

void __noinline__ bench_4(float * out_sample, unsigned int n)
{
    float b1 = std::cos(0.01);
    float y1 = 0;
    float y2 = 1;

    for (int i = n; i != 0; --i)
    {
        float y0 = b1 * y1 - y2;
        *out_sample++ = y0;
    }
}

void __noinline__ bench_5(float * out_sample, unsigned int n)
{
    float b1 = std::cos(0.01);
    float y1 = 0;
    float y2 = 1;

    for (int i = 0; i != n; ++i)
    {
        float y0 = b1 * y1 - y2;
        out_sample[i] = y0;
    }
}

void __noinline__ bench_6(float * out_sample, unsigned int n)
{
    float b1 = std::cos(0.01);
    float y1 = 0;
    float y2 = 1;

    do
    {
        float y0 = b1 * y1 - y2;
        *out_sample++ = y0;
        --n;
    }
    while (__builtin_expect(n!=0, 1));
}

void __noinline__ bench_7(float * out_sample, unsigned int n)
{
    float b1 = std::cos(0.01);
    float y1 = 0;
    float y2 = 1;

    do
    {
        float y0 = b1 * y1 - y2;
        *out_sample++ = y0;
        --n;
    }
    while (n);
}


void __noinline__ bench_8(float * out_sample, unsigned int n)
{
    float b1 = std::cos(0.01);
    float y1 = 0;
    float y2 = 1;

    float * end_sample = out_sample + n;

    do
    {
        float y0 = b1 * y1 - y2;
        *out_sample++ = y0;
    }
    while (out_sample != end_sample);
}

void __noinline__ bench_9(float * out_sample, unsigned int n)
{
    float b1 = std::cos(0.01);
    float y1 = 0;
    float y2 = 1;

    float * end_sample = out_sample + n;

    do
    {
        float y0 = b1 * y1 - y2;
        *out_sample++ = y0;
    }
    while (__builtin_expect(out_sample != end_sample, true));
}

int main()
{
    using namespace std;

    nova::aligned_array<float, 1024> buffer;
    fill_container(buffer);

    unsigned int iterations = 50000000;

    cout << "while, iterate n and out_sample" << endl;
    run_bench(boost::bind(bench_1, buffer.begin(), 64), iterations);

    cout << endl << "while, iterate n and out_sample, preincrement" << endl;
    run_bench(boost::bind(bench_1a, buffer.begin(), 64), iterations);

    cout << endl << "while, iterate out_sample, test for end_sample" << endl;
    run_bench(boost::bind(bench_2, buffer.begin(), 64), iterations);

    cout << endl << "for, iterate i and out_sample" << endl;
    run_bench(boost::bind(bench_3, buffer.begin(), 64), iterations);

    cout << endl << "for, iterate i and out_sample, preincrement" << endl;
    run_bench(boost::bind(bench_3a, buffer.begin(), 64), iterations);

    cout << endl << "for, iterate i (n to 0) and out_sample" << endl;
    run_bench(boost::bind(bench_4, buffer.begin(), 64), iterations);

    cout << endl << "for, iterate i" << endl;
    run_bench(boost::bind(bench_5, buffer.begin(), 64), iterations);

    cout << endl << "do, iterate n and out_sample, branch hinting" << endl;
    run_bench(boost::bind(bench_6, buffer.begin(), 64), iterations);

    cout << endl << "do, iterate n and out_sample" << endl;
    run_bench(boost::bind(bench_7, buffer.begin(), 64), iterations);

    cout << endl << "do, iterate out_sample, test for end_sample" << endl;
    run_bench(boost::bind(bench_8, buffer.begin(), 64), iterations);

    cout << endl << "do, iterate out_sample, test for end_sample, branch hinting" << endl;
    run_bench(boost::bind(bench_9, buffer.begin(), 64), iterations);
    cout << endl;
}
