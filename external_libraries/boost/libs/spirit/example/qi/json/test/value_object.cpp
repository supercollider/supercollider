/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 *   Copyright (C) 2010, 2011 Object Modeling Designs
 */

#include <boost/detail/lightweight_test.hpp>
#include "json/value.hpp"
#include "json/io.hpp"

namespace json = ciere::json;

void const_test( json::value const & value );

int main()
{
   json::value value;

   BOOST_TEST( value.type() == json::null_type );

   value = ciere::json::object()
      ("number" ,123)
      ("foo"    ,"bar")
      ("meaning", 42)
      ;

   BOOST_TEST( value.type() == json::object_type );


   BOOST_TEST(value["number"]  == 123);
   BOOST_TEST(value["foo"]     == "bar");
   BOOST_TEST(value["meaning"] == 42);

   value["number"] = 78.3;
   BOOST_TEST(value["number"]  == 78.3);

   value["foo"] = ciere::json::object()
      ( "color",  "black" )
      ( "size" ,  93      )
      ( "tone" ,  ciere::json::null_t() )
      ;

   BOOST_TEST(value["foo"]["size"] == 93);
   BOOST_TEST(value["foo"]["color"] == "black");

   value["foo"]["color"] = "blue";
   BOOST_TEST(value["foo"]["color"] != "black");
   BOOST_TEST(value["foo"]["color"] == "blue");

   BOOST_TEST(value["foo"].length() == 3);

   json::value::object_iterator iter     = value["foo"].begin_object();
   json::value::object_iterator iter_end = value["foo"].end_object();
   BOOST_TEST( iter->name()  == "color" );
   BOOST_TEST( iter->value() == "blue" );
   ++iter;
   BOOST_TEST( iter->name()  == "size" );
   BOOST_TEST( iter->value() == 93 );
   ++iter;
   BOOST_TEST( iter->name()  == "tone" );
   BOOST_TEST( iter->value() == json::null_t() );
   ++iter;
   BOOST_TEST( iter == iter_end );

   value.set("bar", json::array()(42)("top")(1234.5) );
   BOOST_TEST( value.has_key("bar") );
   BOOST_TEST( !value.has_key("cafe") );
   BOOST_TEST( value["bar"][2] == 1234.5 );

   value.set("cafe", "open");
   BOOST_TEST(value["cafe"] == "open" );

   const_test(value);

   return boost::report_errors();
}



void const_test( json::value const & value )
{
   BOOST_TEST( value.has_key("cafe") );

   try
   {
      value["sam"];
      int should_have_thrown = 1;
      BOOST_TEST( should_have_thrown == 0 );
   }
   catch( std::out_of_range & e )
   {}

   BOOST_TEST( value["cafe"] == "open" );
   BOOST_TEST( value["bar"][2] == 1234.5 );
}

