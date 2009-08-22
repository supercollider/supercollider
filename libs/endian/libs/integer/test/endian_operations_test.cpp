//  endian_operations_test.cpp  ----------------------------------------------//

//  Copyright Beman Dawes 2008

//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/endian

//----------------------------------------------------------------------------//

//  This test probes operator overloading, including interaction between
//  operand types.

//  See endian_test for tests of endianess correctness, size, and value.

//----------------------------------------------------------------------------//

#define BOOST_ENDIAN_LOG

#include <boost/integer/endian.hpp>
#include <cassert>
#include <iostream>

namespace bi = boost::integer;

#ifdef _MSC_VER
# pragma warning( disable : 4244 )  // conversion ..., possible loss of data
# pragma warning( disable : 4018 )  // signed/unsigned mismatch
#endif

template <class T1,  class T2>
struct default_construct
{
  static void test()
  {
    T1 o1;
    o1 = 1; // quiet warnings
  }
};

template <class T1,  class T2>
struct construct
{
  static void test()
  {
    T2 o2(1);
    T1 o1(o2);
  }
};

template <class T1,  class T2>
struct initialize
{
  static void test()
  {
    T1 o2(2);
    T1 o1 = o2;
  }
};

template <class T1,  class T2>
struct assign
{
  static void test()
  {
    T2 o2;
    o2 = 1;
    T1 o1;
    o1 = o2;
  }
};

template <class T1,  class T2>
struct relational
{
  static void test()
  {
    T1 o1(1);
    T2 o2(2);
    if ( o1 == o2 ) return;
    if ( o1 != o2 ) return;
    if ( o1 < o2 ) return;
    if ( o1 <= o2 ) return;
    if ( o1 > o2 ) return;
    if ( o1 >= o2 ) return;
  }
};

template <class T1,  class T2>
struct op_plus
{
  static void test()
  {
    T1 o1(1);
    T2 o2(2);
    T1 o3;

    o3 = +o1;

    o3 = o1 + o2;

    o1 += o2;
  }
};

template <class T1,  class T2>
struct op_star
{
  static void test()
  {
    T1 o1(1);
    T2 o2(2);
    T1 o3;

    o3 = o1 * o2;

    o1 *= o2;
  }
};

template <template<class,  class> class Test,  class T1>
void op_test_aux()
{
  Test<T1, char>::test();
  Test<T1, unsigned char>::test();
  Test<T1, signed char>::test();
  Test<T1, short>::test();
  Test<T1, unsigned short>::test();
  Test<T1, int>::test();
  Test<T1, unsigned int>::test();
  Test<T1, long>::test();
  Test<T1, unsigned long>::test();
  Test<T1, long long>::test();
  Test<T1, unsigned long long>::test();
  Test<T1, bi::big8_t>::test();
  Test<T1, bi::big16_t>::test();
  Test<T1, bi::big24_t>::test();
  Test<T1, bi::big32_t>::test();
  Test<T1, bi::big40_t>::test();
  Test<T1, bi::big48_t>::test();
  Test<T1, bi::big56_t>::test();
  Test<T1, bi::big64_t>::test();
  Test<T1, bi::ubig8_t>::test();
  Test<T1, bi::ubig16_t>::test();
  Test<T1, bi::ubig24_t>::test();
  Test<T1, bi::ubig32_t>::test();
  Test<T1, bi::ubig40_t>::test();
  Test<T1, bi::ubig48_t>::test();
  Test<T1, bi::ubig56_t>::test();
  Test<T1, bi::ubig64_t>::test();
  Test<T1, bi::little8_t>::test();
  Test<T1, bi::little16_t>::test();
  Test<T1, bi::little24_t>::test();
  Test<T1, bi::little32_t>::test();
  Test<T1, bi::little40_t>::test();
  Test<T1, bi::little48_t>::test();
  Test<T1, bi::little56_t>::test();
  Test<T1, bi::little64_t>::test();
  Test<T1, bi::ulittle8_t>::test();
  Test<T1, bi::ulittle16_t>::test();
  Test<T1, bi::ulittle24_t>::test();
  Test<T1, bi::ulittle32_t>::test();
  Test<T1, bi::ulittle40_t>::test();
  Test<T1, bi::ulittle48_t>::test();
  Test<T1, bi::ulittle56_t>::test();
  Test<T1, bi::ulittle64_t>::test();
  Test<T1, bi::native8_t>::test();
  Test<T1, bi::native16_t>::test();
  Test<T1, bi::native24_t>::test();
  Test<T1, bi::native32_t>::test();
  Test<T1, bi::native40_t>::test();
  Test<T1, bi::native48_t>::test();
  Test<T1, bi::native56_t>::test();
  Test<T1, bi::native64_t>::test();
  Test<T1, bi::unative8_t>::test();
  Test<T1, bi::unative16_t>::test();
  Test<T1, bi::unative24_t>::test();
  Test<T1, bi::unative32_t>::test();
  Test<T1, bi::unative40_t>::test();
  Test<T1, bi::unative48_t>::test();
  Test<T1, bi::unative56_t>::test();
  Test<T1, bi::unative64_t>::test();

}

template <template<class,  class> class Test>
void op_test()
{
  op_test_aux<Test, char>();
  op_test_aux<Test, unsigned char>();
  op_test_aux<Test, signed char>();
  op_test_aux<Test, short>();
  op_test_aux<Test, unsigned short>();
  op_test_aux<Test, int>();
  op_test_aux<Test, unsigned int>();
  op_test_aux<Test, long>();
  op_test_aux<Test, unsigned long>();
  op_test_aux<Test, long long>();
  op_test_aux<Test, unsigned long long>();
  op_test_aux<Test, bi::big8_t>();
  op_test_aux<Test, bi::big16_t>();
  op_test_aux<Test, bi::big24_t>();
  op_test_aux<Test, bi::big32_t>();
  op_test_aux<Test, bi::big40_t>();
  op_test_aux<Test, bi::big48_t>();
  op_test_aux<Test, bi::big56_t>();
  op_test_aux<Test, bi::big64_t>();
  op_test_aux<Test, bi::ubig8_t>();
  op_test_aux<Test, bi::ubig16_t>();
  op_test_aux<Test, bi::ubig24_t>();
  op_test_aux<Test, bi::ubig32_t>();
  op_test_aux<Test, bi::ubig40_t>();
  op_test_aux<Test, bi::ubig48_t>();
  op_test_aux<Test, bi::ubig56_t>();
  op_test_aux<Test, bi::ubig64_t>();
  op_test_aux<Test, bi::little8_t>();
  op_test_aux<Test, bi::little16_t>();
  op_test_aux<Test, bi::little24_t>();
  op_test_aux<Test, bi::little32_t>();
  op_test_aux<Test, bi::little40_t>();
  op_test_aux<Test, bi::little48_t>();
  op_test_aux<Test, bi::little56_t>();
  op_test_aux<Test, bi::little64_t>();
  op_test_aux<Test, bi::ulittle8_t>();
  op_test_aux<Test, bi::ulittle16_t>();
  op_test_aux<Test, bi::ulittle24_t>();
  op_test_aux<Test, bi::ulittle32_t>();
  op_test_aux<Test, bi::ulittle40_t>();
  op_test_aux<Test, bi::ulittle48_t>();
  op_test_aux<Test, bi::ulittle56_t>();
  op_test_aux<Test, bi::ulittle64_t>();
  op_test_aux<Test, bi::native8_t>();
  op_test_aux<Test, bi::native16_t>();
  op_test_aux<Test, bi::native24_t>();
  op_test_aux<Test, bi::native32_t>();
  op_test_aux<Test, bi::native40_t>();
  op_test_aux<Test, bi::native48_t>();
  op_test_aux<Test, bi::native56_t>();
  op_test_aux<Test, bi::native64_t>();
  op_test_aux<Test, bi::unative8_t>();
  op_test_aux<Test, bi::unative16_t>();
  op_test_aux<Test, bi::unative24_t>();
  op_test_aux<Test, bi::unative32_t>();
  op_test_aux<Test, bi::unative40_t>();
  op_test_aux<Test, bi::unative48_t>();
  op_test_aux<Test, bi::unative56_t>();
  op_test_aux<Test, bi::unative64_t>();
}

int main()
{
  bi::endian_log = false;

  //  make sure some simple things work

  bi::big32_t o1(1);
  bi::big32_t o2(2L);
  bi::big32_t o3(3LL);
  bi::big64_t o4(1);

  //  use cases; if BOOST_ENDIAN_LOG is defined, will output to clog info on
  //  what overloads and conversions are actually being performed.

  bi::endian_log = true;

  std::clog << "set up test values\n";
  bi::big32_t      big(12345);
  bi::ulittle16_t  ulittle(10);
  bi::big64_t      result;


  std::clog << "\nresult = +big\n";
  result = +big;

  std::clog << "\nresult = -big\n";
  result = -big;

  std::clog << "\n++big\n";
  ++big;

  std::clog << "\nresult = big++\n";
  big++;

  std::clog << "\n--big\n";
  --big;

  std::clog << "\nbig--\n";
  big--;

  std::clog << "\nresult = big * big\n";
  result = big * big;

  std::clog << "\nresult = big * big\n";
  result = big * big;

  std::clog << "\nresult = big * ulittle\n";
  result = big * ulittle;

  std::clog << "\nbig *= ulittle\n";
  big *= ulittle;

  std::clog << "\nresult = ulittle * big\n";
  result = ulittle * big;

  std::clog << "\nresult = big * 5\n";
  result = big * 5;

  std::clog << "\nbig *= 5\n";
  big *= 5;

  std::clog << "\nresult = 5 * big\n";
  result = 5 * big;

  std::clog << "\nresult = ulittle * 5\n";
  result = ulittle * 5;

  std::clog << "\nresult = 5 * ulittle\n";
  result = 5 * ulittle;

  std::clog << "\nresult = 5 * 10\n";
  result = 5 * 10;
  std::clog << "\n";

  bi::endian_log = false;
  
  //  perform the indicated test on ~60*60 operand types

  op_test<default_construct>();
  op_test<construct>();  // includes copy construction
  op_test<initialize>();
  op_test<assign>();
  op_test<relational>();
  op_test<op_plus>();
  op_test<op_star>();

  return 0;
}
