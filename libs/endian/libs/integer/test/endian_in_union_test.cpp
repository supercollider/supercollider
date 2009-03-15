//  endian_in_union_test.cpp  -------------------------------------------------//

//  Copyright Beman Dawes 2008

//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/endian

//----------------------------------------------------------------------------//

#define BOOST_ENDIANS_IN_UNIONS

#include <boost/integer/endian.hpp>
#include <cassert>

using namespace boost::integer;

union U
{
  big8_t           big8;
  big16_t          big16;
  big24_t          big24;
  big32_t          big32;
  big40_t          big40;
  big48_t          big48;
  big56_t          big56;
  big64_t          big64;
                   
  ubig8_t          ubig8;
  ubig16_t         ubig16;
  ubig24_t         ubig24;
  ubig32_t         ubig32;
  ubig40_t         ubig40;
  ubig48_t         ubig48;
  ubig56_t         ubig56;
  ubig64_t         ubig64;
                   
  little8_t        little8;
  little16_t       little16;
  little24_t       little24;
  little32_t       little32;
  little40_t       little40;
  little48_t       little48;
  little56_t       little56;
  little64_t       little64;
                   
  ulittle8_t       ulittle8;
  ulittle16_t      ulittle16;
  ulittle24_t      ulittle24;
  ulittle32_t      ulittle32;
  ulittle40_t      ulittle40;
  ulittle48_t      ulittle48;
  ulittle56_t      ulittle56;
  ulittle64_t      ulittle64;
                   
  native8_t        native8;
  native16_t       native16;
  native24_t       native24;
  native32_t       native32;
  native40_t       native40;
  native48_t       native48;
  native56_t       native56;
  native64_t       native64;
                   
  unative8_t       unative8;
  unative16_t      unative16;
  unative24_t      unative24;
  unative32_t      unative32;
  unative40_t      unative40;
  unative48_t      unative48;
  unative56_t      unative56;
  unative64_t      unative64;
};

U foo;

int main()
{

  return 0;
}

