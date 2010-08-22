//  endian_binary_stream_test.cpp  -----------------------------------------------------//

//  Copyright Beman Dawes 2009

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/integer/endian_binary_stream.hpp>
#include <iostream>
#include <sstream>

using namespace boost;
using namespace boost::integer;
using namespace std;

namespace
{
  int errors = 0;

  void verify( bool x, const char * file, int line )
  {
    if ( x ) return;
    ++errors;
    cout << file << "(" << line << ") : error: predicate is false\n" << endl;
  }

}

#define BOOST_VERIFY(predicate) verify( predicate, __FILE__, __LINE__ )

int main()
{
  std::stringstream ss( std::ios_base::in | std::ios_base::out | std::ios_base::binary );

  //  big tests

  big8_t big8_t_1(0x01), big8_t_2;
  ss <= big8_t_1;
  ss >= big8_t_2;
  BOOST_VERIFY( big8_t_1 == big8_t_2 );

  big16_t big16_t_1(0x0102), big16_t_2;
  ss <= big16_t_1;
  ss >= big16_t_2;
  BOOST_VERIFY( big16_t_1 == big16_t_2 );

  big24_t big24_t_1(0x010203L), big24_t_2;
  ss <= big24_t_1;
  ss >= big24_t_2;
  BOOST_VERIFY( big24_t_1 == big24_t_2 );

  big32_t big32_t_1(0x01020304L), big32_t_2;
  ss <= big32_t_1;
  ss >= big32_t_2;
  BOOST_VERIFY( big32_t_1 == big32_t_2 );

  big40_t big40_t_1(0x0102030405LL), big40_t_2;
  ss <= big40_t_1;
  ss >= big40_t_2;
  BOOST_VERIFY( big40_t_1 == big40_t_2 );

  big48_t big48_t_1(0x010203040506LL), big48_t_2;
  ss <= big48_t_1;
  ss >= big48_t_2;
  BOOST_VERIFY( big48_t_1 == big48_t_2 );

  big56_t big56_t_1(0x01020304050607LL), big56_t_2;
  ss <= big56_t_1;
  ss >= big56_t_2;
  BOOST_VERIFY( big56_t_1 == big56_t_2 );

  big64_t big64_t_1(0x0102030405060808LL), big64_t_2;
  ss <= big64_t_1;
  ss >= big64_t_2;
  BOOST_VERIFY( big64_t_1 == big64_t_2 );

  //  ubig tests

  ubig8_t ubig8_t_1(0x01), ubig8_t_2;
  ss <= ubig8_t_1;
  ss >= ubig8_t_2;
  BOOST_VERIFY( ubig8_t_1 == ubig8_t_2 );

  ubig16_t ubig16_t_1(0x0102), ubig16_t_2;
  ss <= ubig16_t_1;
  ss >= ubig16_t_2;
  BOOST_VERIFY( ubig16_t_1 == ubig16_t_2 );

  ubig24_t ubig24_t_1(0x010203L), ubig24_t_2;
  ss <= ubig24_t_1;
  ss >= ubig24_t_2;
  BOOST_VERIFY( ubig24_t_1 == ubig24_t_2 );

  ubig32_t ubig32_t_1(0x01020304L), ubig32_t_2;
  ss <= ubig32_t_1;
  ss >= ubig32_t_2;
  BOOST_VERIFY( ubig32_t_1 == ubig32_t_2 );

  ubig40_t ubig40_t_1(0x0102030405LL), ubig40_t_2;
  ss <= ubig40_t_1;
  ss >= ubig40_t_2;
  BOOST_VERIFY( ubig40_t_1 == ubig40_t_2 );

  ubig48_t ubig48_t_1(0x010203040506LL), ubig48_t_2;
  ss <= ubig48_t_1;
  ss >= ubig48_t_2;
  BOOST_VERIFY( ubig48_t_1 == ubig48_t_2 );

  ubig56_t ubig56_t_1(0x01020304050607LL), ubig56_t_2;
  ss <= ubig56_t_1;
  ss >= ubig56_t_2;
  BOOST_VERIFY( ubig56_t_1 == ubig56_t_2 );

  ubig64_t ubig64_t_1(0x0102030405060808LL), ubig64_t_2;
  ss <= ubig64_t_1;
  ss >= ubig64_t_2;
  BOOST_VERIFY( ubig64_t_1 == ubig64_t_2 );

  //  little tests

  little8_t little8_t_1(0x01), little8_t_2;
  ss <= little8_t_1;
  ss >= little8_t_2;
  BOOST_VERIFY( little8_t_1 == little8_t_2 );

  little16_t little16_t_1(0x0102), little16_t_2;
  ss <= little16_t_1;
  ss >= little16_t_2;
  BOOST_VERIFY( little16_t_1 == little16_t_2 );

  little24_t little24_t_1(0x010203L), little24_t_2;
  ss <= little24_t_1;
  ss >= little24_t_2;
  BOOST_VERIFY( little24_t_1 == little24_t_2 );

  little32_t little32_t_1(0x01020304L), little32_t_2;
  ss <= little32_t_1;
  ss >= little32_t_2;
  BOOST_VERIFY( little32_t_1 == little32_t_2 );

  little40_t little40_t_1(0x0102030405LL), little40_t_2;
  ss <= little40_t_1;
  ss >= little40_t_2;
  BOOST_VERIFY( little40_t_1 == little40_t_2 );

  little48_t little48_t_1(0x010203040506LL), little48_t_2;
  ss <= little48_t_1;
  ss >= little48_t_2;
  BOOST_VERIFY( little48_t_1 == little48_t_2 );

  little56_t little56_t_1(0x01020304050607LL), little56_t_2;
  ss <= little56_t_1;
  ss >= little56_t_2;
  BOOST_VERIFY( little56_t_1 == little56_t_2 );

  little64_t little64_t_1(0x0102030405060808LL), little64_t_2;
  ss <= little64_t_1;
  ss >= little64_t_2;
  BOOST_VERIFY( little64_t_1 == little64_t_2 );

  //  ulittle tests

  ulittle8_t ulittle8_t_1(0x01), ulittle8_t_2;
  ss <= ulittle8_t_1;
  ss >= ulittle8_t_2;
  BOOST_VERIFY( ulittle8_t_1 == ulittle8_t_2 );

  ulittle16_t ulittle16_t_1(0x0102), ulittle16_t_2;
  ss <= ulittle16_t_1;
  ss >= ulittle16_t_2;
  BOOST_VERIFY( ulittle16_t_1 == ulittle16_t_2 );

  ulittle24_t ulittle24_t_1(0x010203L), ulittle24_t_2;
  ss <= ulittle24_t_1;
  ss >= ulittle24_t_2;
  BOOST_VERIFY( ulittle24_t_1 == ulittle24_t_2 );

  ulittle32_t ulittle32_t_1(0x01020304L), ulittle32_t_2;
  ss <= ulittle32_t_1;
  ss >= ulittle32_t_2;
  BOOST_VERIFY( ulittle32_t_1 == ulittle32_t_2 );

  ulittle40_t ulittle40_t_1(0x0102030405LL), ulittle40_t_2;
  ss <= ulittle40_t_1;
  ss >= ulittle40_t_2;
  BOOST_VERIFY( ulittle40_t_1 == ulittle40_t_2 );

  ulittle48_t ulittle48_t_1(0x010203040506LL), ulittle48_t_2;
  ss <= ulittle48_t_1;
  ss >= ulittle48_t_2;
  BOOST_VERIFY( ulittle48_t_1 == ulittle48_t_2 );

  ulittle56_t ulittle56_t_1(0x01020304050607LL), ulittle56_t_2;
  ss <= ulittle56_t_1;
  ss >= ulittle56_t_2;
  BOOST_VERIFY( ulittle56_t_1 == ulittle56_t_2 );

  ulittle64_t ulittle64_t_1(0x0102030405060808LL), ulittle64_t_2;
  ss <= ulittle64_t_1;
  ss >= ulittle64_t_2;
  BOOST_VERIFY( ulittle64_t_1 == ulittle64_t_2 );

  //  native tests

  native8_t native8_t_1(0x01), native8_t_2;
  ss <= native8_t_1;
  ss >= native8_t_2;
  BOOST_VERIFY( native8_t_1 == native8_t_2 );

  native16_t native16_t_1(0x0102), native16_t_2;
  ss <= native16_t_1;
  ss >= native16_t_2;
  BOOST_VERIFY( native16_t_1 == native16_t_2 );

  native24_t native24_t_1(0x010203L), native24_t_2;
  ss <= native24_t_1;
  ss >= native24_t_2;
  BOOST_VERIFY( native24_t_1 == native24_t_2 );

  native32_t native32_t_1(0x01020304L), native32_t_2;
  ss <= native32_t_1;
  ss >= native32_t_2;
  BOOST_VERIFY( native32_t_1 == native32_t_2 );

  native40_t native40_t_1(0x0102030405LL), native40_t_2;
  ss <= native40_t_1;
  ss >= native40_t_2;
  BOOST_VERIFY( native40_t_1 == native40_t_2 );

  native48_t native48_t_1(0x010203040506LL), native48_t_2;
  ss <= native48_t_1;
  ss >= native48_t_2;
  BOOST_VERIFY( native48_t_1 == native48_t_2 );

  native56_t native56_t_1(0x01020304050607LL), native56_t_2;
  ss <= native56_t_1;
  ss >= native56_t_2;
  BOOST_VERIFY( native56_t_1 == native56_t_2 );

  native64_t native64_t_1(0x0102030405060808LL), native64_t_2;
  ss <= native64_t_1;
  ss >= native64_t_2;
  BOOST_VERIFY( native64_t_1 == native64_t_2 );

  //  unative tests

  unative8_t unative8_t_1(0x01), unative8_t_2;
  ss <= unative8_t_1;
  ss >= unative8_t_2;
  BOOST_VERIFY( unative8_t_1 == unative8_t_2 );

  unative16_t unative16_t_1(0x0102), unative16_t_2;
  ss <= unative16_t_1;
  ss >= unative16_t_2;
  BOOST_VERIFY( unative16_t_1 == unative16_t_2 );

  unative24_t unative24_t_1(0x010203L), unative24_t_2;
  ss <= unative24_t_1;
  ss >= unative24_t_2;
  BOOST_VERIFY( unative24_t_1 == unative24_t_2 );

  unative32_t unative32_t_1(0x01020304L), unative32_t_2;
  ss <= unative32_t_1;
  ss >= unative32_t_2;
  BOOST_VERIFY( unative32_t_1 == unative32_t_2 );

  unative40_t unative40_t_1(0x0102030405LL), unative40_t_2;
  ss <= unative40_t_1;
  ss >= unative40_t_2;
  BOOST_VERIFY( unative40_t_1 == unative40_t_2 );

  unative48_t unative48_t_1(0x010203040506LL), unative48_t_2;
  ss <= unative48_t_1;
  ss >= unative48_t_2;
  BOOST_VERIFY( unative48_t_1 == unative48_t_2 );

  unative56_t unative56_t_1(0x01020304050607LL), unative56_t_2;
  ss <= unative56_t_1;
  ss >= unative56_t_2;
  BOOST_VERIFY( unative56_t_1 == unative56_t_2 );

  unative64_t unative64_t_1(0x0102030405060808LL), unative64_t_2;
  ss <= unative64_t_1;
  ss >= unative64_t_2;
  BOOST_VERIFY( unative64_t_1 == unative64_t_2 );

  cout << errors << " error(s) detected\n";
  return errors;
}
