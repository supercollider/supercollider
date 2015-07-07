/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 */

#include <boost/detail/lightweight_test.hpp>
#include "json/value.hpp"
#include <string>

#define SHOULD_HAVE_THROWN 0

namespace json = ciere::json;

int main()
{
   json::value value;

   {
      value = "test";
      std::string tmp = value.get<json::string_t>();
      BOOST_TEST(tmp == "test");

      try
      {
         value.get<json::int_t>();
         BOOST_TEST(SHOULD_HAVE_THROWN);
      }
      catch(...){}
   }


   {
      value = 42;
      int tmp = value.get<json::int_t>();
      BOOST_TEST(tmp == 42);

      try
      {
         value.get<json::float_t>();
         BOOST_TEST(SHOULD_HAVE_THROWN);
      }
      catch(...){}
   }


   {
      value = 42.3;
      double tmp = value.get<json::float_t>();
      BOOST_TEST(tmp == 42.3);

      try
      {
         value.get<json::int_t>();
         BOOST_TEST(SHOULD_HAVE_THROWN);
      }
      catch(...){}
   }



   {
      value = false;
      bool tmp = value.get<json::bool_t>();
      BOOST_TEST(tmp == false);

      try
      {
         value.get<json::int_t>();
         BOOST_TEST(SHOULD_HAVE_THROWN);
      }
      catch(...){}
   }


   return boost::report_errors();
}

