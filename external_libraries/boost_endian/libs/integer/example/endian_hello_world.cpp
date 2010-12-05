//  endian_io_test.cpp  ----------------------------------------------------------------//

//  Copyright Beman Dawes, 2009

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See library home page at http://www.boost.org/libs/endian

#include <boost/integer/endian.hpp>
#include <boost/integer/endian_binary_stream.hpp>
#include <boost/binary_stream.hpp>
#include <iostream>

using namespace boost;
using namespace boost::integer;

int main()
{
  int_least32_t v = 0x31323334L;  // = ASCII { '1', '2', '3', '4' }
                                  // value chosen to work on text stream
  big32_t    b(v);
  little32_t l(v);

  std::cout << "Hello, endian world!\n\n";

  std::cout << v << ' ' << b << ' ' << l << '\n';
  std::cout <= v <= ' ' <= b <= ' ' <= l <= '\n';
}

