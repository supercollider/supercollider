/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 *   Copyright (C) 2010, 2011 Object Modeling Designs
 */

#include <boost/detail/lightweight_test.hpp>
#include "json/value.hpp"

int main()
{
   ciere::json::value value;

   value = "test";
   BOOST_TEST(value == "test");

   value = 42;
   BOOST_TEST(value == 42);

   value = 42.3;
   BOOST_TEST(value == 42.3);

   value = false;
   BOOST_TEST(value == false);

   value = ciere::json::null_t();
   BOOST_TEST(value == ciere::json::null_t());

   value = ciere::json::array()(123)("foo")("bar")(42.5);
   BOOST_TEST(value[1] == "foo");
   BOOST_TEST(value[3] == 42.5);

   value = ciere::json::object()
      ("number" ,123)
      ("foo"    ,"bar")
      ("meaning", 42);
   BOOST_TEST(value["number"]  == 123);
   BOOST_TEST(value["foo"]     == "bar");
   BOOST_TEST(value["meaning"] == 42);

   return boost::report_errors();
}

