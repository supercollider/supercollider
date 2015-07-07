/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 */

#include <string>
#include <boost/detail/lightweight_test.hpp>
#include "json/value.hpp"

namespace json = ciere::json;

void null_test()
{
   // default constructs to null
   json::value v1;
   json::value v2;
   BOOST_TEST(v1 == v2);
   BOOST_TEST(v1.type() == json::null_type);
   BOOST_TEST(v1 == json::null_t());

   // should throw with not a container
   try
   {
      v1.length();
      BOOST_TEST( "throw fail" == 0 );
   }
   catch( json::not_container_type & )
   {}
   catch(...)
   {
      BOOST_TEST( "throw wrong type" == 0 );
   }
}


void bool_test()
{
   json::value v1(true);
   BOOST_TEST(v1.type() == json::bool_type);

   json::value v2(false);
   BOOST_TEST(v1.type() == json::bool_type);

   BOOST_TEST(v1 != v2);
   BOOST_TEST(v1 == true);
   BOOST_TEST(v2 == false);

   v1 = false;
   BOOST_TEST(v1 == v2);

   v1 = true;
   BOOST_TEST(v1 != false);


   // should throw with not a container
   try
   {
      v1.length();
      BOOST_TEST( "throw fail" == 0 );
   }
   catch( json::not_container_type & )
   {}
   catch(...)
   {
      BOOST_TEST( "throw wrong type" == 0 );
   }
}


void string_test()
{
   json::value v1("test");
   BOOST_TEST(v1.type() == json::string_type);
   BOOST_TEST(v1 == "test");

   std::string s1("foo");
   json::value v2(s1);
   BOOST_TEST(v2 == s1);

   v1 = s1;
   BOOST_TEST(v1 == v2);

   v1 = "foo";
   BOOST_TEST(v1 == v2);

   v2 = "gorp";
   BOOST_TEST(v1 != v2);
   BOOST_TEST(v1 != std::string("bar"));

   // should throw with not a container
   try
   {
      v1.length();
      BOOST_TEST( "throw fail" == 0 );
   }
   catch( json::not_container_type & )
   {}
   catch(...)
   {
      BOOST_TEST( "throw wrong type" == 0 );
   }
}


void int_test()
{
   json::value v1(12);
   BOOST_TEST(v1.type() == json::int_type);
   BOOST_TEST(v1 == 12);

   v1 = uint8_t(8);
   BOOST_TEST(v1.type() == json::int_type);
   BOOST_TEST(v1 == 8);

   v1 = int8_t(-10);
   BOOST_TEST(v1.type() == json::int_type);
   BOOST_TEST(v1 == -10);

   v1 = uint16_t(923);
   BOOST_TEST(v1.type() == json::int_type);
   BOOST_TEST(v1 == 923);

   v1 = int16_t(-234);
   BOOST_TEST(v1.type() == json::int_type);
   BOOST_TEST(v1 == -234);

   v1 = uint32_t(98);
   BOOST_TEST(v1.type() == json::int_type);
   BOOST_TEST(v1 == 98);

   v1 = int32_t(-8);
   BOOST_TEST(v1.type() == json::int_type);
   BOOST_TEST(v1 == -8);

   json::value v2(-8);
   BOOST_TEST(v1 == v2);
   BOOST_TEST(v1 == -8.0);

   // should throw with not a container
   try
   {
      v1.length();
      BOOST_TEST( "throw fail" == 0 );
   }
   catch( json::not_container_type & )
   {}
   catch(...)
   {
      BOOST_TEST( "throw wrong type" == 0 );
   }
}


void float_test()
{
   json::value v1(1.0);
   BOOST_TEST(v1.type() == json::double_type);
   BOOST_TEST(v1 == 1.0);

   v1 = 987.5;
   BOOST_TEST(v1 == 987.5);
   BOOST_TEST(v1 != 987.1);

   json::value v2(987.5);
   BOOST_TEST(v1 == v2);

   double d1 = 4.56342124;
   json::value v3(d1);
   BOOST_TEST(v3.type() == json::double_type);
   BOOST_TEST(v3 == d1);
   BOOST_TEST(v3 != v1);

   v1 = d1;
   BOOST_TEST(v1 == v3);

   float f1 = 123.4567;
   json::value v4(f1);
   BOOST_TEST(v4.type() == json::double_type);
   BOOST_TEST(v4 == f1);
   BOOST_TEST(v4 != v1);
   v1 = f1;
   BOOST_TEST(v1 == v4);

   // should throw with not a container
   try
   {
      v1.length();
      BOOST_TEST( "throw fail" == 0 );
   }
   catch( json::not_container_type & )
   {}
   catch(...)
   {
      BOOST_TEST( "throw wrong type" == 0 );
   }
}



void value_test()
{
   json::value v1(1234);
   json::value v2(v1);
   BOOST_TEST(v2.type() == json::int_type);
   BOOST_TEST(v2 == 1234);

   // should throw with not a container
   try
   {
      v1.length();
      BOOST_TEST( "throw fail" == 0 );
   }
   catch( json::not_container_type & )
   {}
   catch(...)
   {
      BOOST_TEST( "throw wrong type" == 0 );
   }
}




int main()
{
   null_test();
   bool_test();
   int_test();
   float_test();
   value_test();
   return boost::report_errors();
}

