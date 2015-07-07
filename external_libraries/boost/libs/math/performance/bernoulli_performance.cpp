//  (C) Copyright John Maddock 2013.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/math/special_functions/bernoulli.hpp>
#include <boost/chrono.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

template <class Clock>
struct stopwatch
{
   typedef typename Clock::duration duration;
   stopwatch()
   {
      m_start = Clock::now();
   }
   duration elapsed()
   {
      return Clock::now() - m_start;
   }
   void reset()
   {
      m_start = Clock::now();
   }

private:
   typename Clock::time_point m_start;
};

void time()
{
   stopwatch<boost::chrono::high_resolution_clock> w;
   //
   // Sum the first 1000 Bernoulli numbers:
   //
   boost::multiprecision::cpp_dec_float_50 sum = 0;
   for(unsigned i = 0; i < 1000; ++i)
   {
      sum += boost::math::bernoulli_b2n<boost::multiprecision::cpp_dec_float_50>(i);
   }
   double t = boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   std::cout << "Total execution time = " << std::setprecision(3) << t << "s" << std::endl;
   std::cout << "Sum was: " << sum << std::endl;
}

int main()
{
   // Call time() twice: first call results in the cache being populated, second used already cached values:
   time();
   time();
   return 0;
}

/*

No atomic int:

Total execution time = 0.646s
Sum was: 2.86e+4134
Total execution time = 0.000341s
Sum was: 2.86e+4134

With atomic int:

Total execution time = 0.643s
Sum was: 2.86e+4134
Total execution time = 0.000309s
Sum was: 2.86e+4134

No threads:

Total execution time = 0.652s
Sum was: 2.86e+4134
Total execution time = 0.000281s
Sum was: 2.86e+4134

*/

