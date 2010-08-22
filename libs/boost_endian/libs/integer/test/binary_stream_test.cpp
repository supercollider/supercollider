//  binary_stream_test.cpp  ------------------------------------------------------------//

//  Copyright Beman Dawes 2009

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/binary_stream.hpp>
#include <string>
#include <iostream>
#include <sstream>

using namespace boost;
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

  short short_1(0x0102), short_2;
  ss <= short_1;
  ss >= short_2;
  BOOST_VERIFY( short_1 == short_2 );

  unsigned short ushort_1(0x0102), ushort_2;
  ss <= ushort_1;
  ss >= ushort_2;
  BOOST_VERIFY( ushort_1 == ushort_2 );

  int int_1(0x01020304), int_2;
  ss <= int_1;
  ss >= int_2;
  BOOST_VERIFY( int_1 == int_2 );

  unsigned int uint_1(0x01020304), uint_2;
  ss <= uint_1;
  ss >= uint_2;
  BOOST_VERIFY( uint_1 == uint_2 );

  long long_1(0x01020304L), long_2;
  ss <= long_1;
  ss >= long_2;
  BOOST_VERIFY( long_1 == long_2 );

  unsigned long ulong_1(0x01020304UL), ulong_2;
  ss <= ulong_1;
  ss >= ulong_2;
  BOOST_VERIFY( ulong_1 == ulong_2 );

  long long long_long_1(0x0102030405060708LL), long_long_2;
  ss <= long_long_1;
  ss >= long_long_2;
  BOOST_VERIFY( long_long_1 == long_long_2 );

  unsigned long long ulong_long_1(0x0102030405060708ULL), ulong_long_2;
  ss <= ulong_long_1;
  ss >= ulong_long_2;
  BOOST_VERIFY( ulong_long_1 == ulong_long_2 );

  float float_1(1.2F), float_2;
  ss <= float_1;
  ss >= float_2;
  BOOST_VERIFY( float_1 == float_2 );

  double double_1(1.2), double_2;
  ss <= double_1;
  ss >= double_2;
  BOOST_VERIFY( double_1 == double_2 );

  long double long_double_1(1.2), long_double_2;
  ss <= long_double_1;
  ss >= long_double_2;
  BOOST_VERIFY( long_double_1 == long_double_2 );

  char char_1(0x01), char_2;
  ss <= char_1;
  ss >= char_2;
  BOOST_VERIFY( char_1 == char_2 );

  signed char schar_1(0x01), schar_2;
  ss <= schar_1;
  ss >= schar_2;
  BOOST_VERIFY( schar_1 == schar_2 );

  unsigned char uchar_1(0x01), uchar_2;
  ss <= uchar_1;
  ss >= uchar_2;
  BOOST_VERIFY( uchar_1 == uchar_2 );

  wchar_t wchar_t_1(L'1'), wchar_t_2;
  ss <= wchar_t_1;
  ss >= wchar_t_2;
  BOOST_VERIFY( wchar_t_1 == wchar_t_2 );

  string string_1("foobar"), string_2;
  ss <= string_1.c_str();
  ss >= string_2;
  BOOST_VERIFY( string_1 == string_2 );
  ss <= string_1;
  ss >= string_2;
  BOOST_VERIFY( string_1 == string_2 );

  cout << errors << " error(s) detected\n";
  return errors;
}
