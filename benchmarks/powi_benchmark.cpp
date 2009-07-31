#include "benchmark_helpers.hpp"

#include <cmath>

template <typename F>
F powi (F x, unsigned long n)
{
   const unsigned long org_n = n;

   F z = 1;
   while (n != 0)
   {
      if ((n & 1) != 0)
      {
         z *= x;
      }
      n >>= 1;
      x *= x;
   }

   return z;
}

double __noinline__ bench_1(double arg, int n)
{
    return powi(arg, n);
}

double __noinline__ bench_2(double arg, int n)
{
    return std::pow(arg, double(n));
}

float __noinline__ bench_3(float arg, int n)
{
    return powi(arg, n);
}

float __noinline__ bench_4(float arg, int n)
{
    return std::pow(arg, float(n));
}

int main(void)
{
    using namespace std;

    const int iterations = 10000000;

    run_bench(boost::bind(bench_1, 2.345, 16), iterations);
    run_bench(boost::bind(bench_2, 2.345, 16), iterations);

    cout << endl;
    run_bench(boost::bind(bench_1, 2.345, 32), iterations);
    run_bench(boost::bind(bench_2, 2.345, 32), iterations);

    cout << endl;
    run_bench(boost::bind(bench_1, 2.345, 64), iterations);
    run_bench(boost::bind(bench_2, 2.345, 64), iterations);

    cout << endl;
    run_bench(boost::bind(bench_1, 2.345, 128), iterations);
    run_bench(boost::bind(bench_2, 2.345, 128), iterations);

    cout << endl;
    run_bench(boost::bind(bench_1, 2.345, 256), iterations);
    run_bench(boost::bind(bench_2, 2.345, 256), iterations);

    cout << endl << endl;
    run_bench(boost::bind(bench_3, 2.345, 16), iterations);
    run_bench(boost::bind(bench_4, 2.345, 16), iterations);

    cout << endl;
    run_bench(boost::bind(bench_3, 2.345, 32), iterations);
    run_bench(boost::bind(bench_4, 2.345, 32), iterations);

    cout << endl;
    run_bench(boost::bind(bench_3, 2.345, 64), iterations);
    run_bench(boost::bind(bench_4, 2.345, 64), iterations);

    cout << endl;
    run_bench(boost::bind(bench_3, 2.345, 128), iterations);
    run_bench(boost::bind(bench_4, 2.345, 128), iterations);

    cout << endl;
    run_bench(boost::bind(bench_3, 2.345, 256), iterations);
    run_bench(boost::bind(bench_4, 2.345, 256), iterations);
}
