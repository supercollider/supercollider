/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 *   Copyright (C) 2010, 2011 Object Modeling Designs
 */

#include <boost/detail/lightweight_test.hpp>
#include "json/value.hpp"
#include "json/io.hpp"
#include "json/parser/grammar_def.hpp"

namespace json = ciere::json;

int main()
{
   json::value value;

   BOOST_TEST( value.type() == json::null_type );

   value = json::construct(
      "{"
      "   \"foo\"  : 42    ,"
      "   \"bar\"  : 498.5 ,"
      "   \"gorp\" : \"ciere\","
      "   \"bork\" : [\"ciere\",23,false]"
      "}"
      );

   BOOST_TEST( value["foo"]  == 42 );
   BOOST_TEST( value["bar"]  == 498.5 );
   BOOST_TEST( value["gorp"] == "ciere" );
   BOOST_TEST( value["bork"][2] == false );

   return boost::report_errors();
}

