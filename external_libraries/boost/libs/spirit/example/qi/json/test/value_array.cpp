/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 *   Copyright (C) 2010, 2011 Object Modeling Designs
 */

#include <boost/detail/lightweight_test.hpp>
#include <iostream>
#include "json/value.hpp"

namespace json = ciere::json;

void const_test( json::value const & value );

void basic_test()
{
   json::value v = json::array()(34)("foo")("bar")(1234.5);
   BOOST_TEST(v.type() == json::array_type);
   BOOST_TEST(v.length() == 4);
   BOOST_TEST(v[1] == "foo");
   v[1] = "gorp";
   BOOST_TEST(v[1] == "gorp");
   BOOST_TEST(v.at(1) == "gorp");
   v.at(1) = "bar";
   BOOST_TEST(v.at(1) == "bar");

   v[8] = json::array()(0)(1)(2);
   BOOST_TEST(v.length() == 9);
   BOOST_TEST(v[8].length() == 3);
   BOOST_TEST(v[8].type() == json::array_type);
   BOOST_TEST(v[8][1] == 1);

   v[8][1] = true;
   BOOST_TEST(v[8][1] == true);
   BOOST_TEST(v[8][1] != false);

   try
   {
      v.at(10); // will throw
      BOOST_TEST("didn't throw" == 0);
   }
   catch(std::out_of_range&){}
   catch(...)
   {
      BOOST_TEST("wrong throw" == 0);
   }
}


void reference_test()
{
   json::value v = json::array()
      (123)("foo")(false)(json::array()
                          (0)(1)(2)(3)(1234.5))
      ("bar")("gorp");

   v[4] = v[3];

   BOOST_TEST(v[4].type() == json::array_type);
   BOOST_TEST(v[4][2] == 2 );

   v[3][2] = "gorp";
   BOOST_TEST(v[3][2] == "gorp");
   BOOST_TEST(v[4][2] != "gorp");
   BOOST_TEST(v[4][2] == 2);

   v[3] = 8;
   BOOST_TEST(v[4] == json::array()(0)(1)(2)(3)(1234.5) );
}


void iterator_test()
{
   json::value v = json::array()(0)(1)(2)(3)(4)(5);
   json::value::array_iterator iter     = v.begin_array();
   json::value::array_iterator iter_end = v.end_array();
   int count = 0;
   while(iter != iter_end)
   {
      BOOST_TEST(*iter == count++);
      *iter = count;
      ++iter;
   }
   BOOST_TEST(count == 6);

   iter     = v.begin_array();
   iter_end = v.end_array();
   count = 1;
   while(iter != iter_end)
   {
      BOOST_TEST(*iter == count++);
      ++iter;
   }
}

void const_test( json::value const & v )
{
   json::value::const_array_iterator iter     = v.begin_array();
   json::value::const_array_iterator iter_end = v.end_array();
   int count = 0;
   while(iter != iter_end)
   {
      BOOST_TEST(*iter == count++);
#ifdef COMPILE_ERROR
      *iter = count;  // should create a compile error
#endif
      ++iter;
   }
   BOOST_TEST(count == 5);
}

void const_test()
{
   json::value v = json::array()(0)(1)(2)(3)(4);
   const_test(v);
}


int main()
{
   basic_test();
   reference_test();
   iterator_test();
   const_test();

   return boost::report_errors();
}




