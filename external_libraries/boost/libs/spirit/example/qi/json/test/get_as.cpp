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
      std::string tmp = value.get_as<json::string_t>();
      BOOST_TEST(tmp == "test");
   }


   {
      value = 42;
      int tmp = value.get_as<json::int_t>();
      BOOST_TEST(tmp == 42);
      std::string tmp_str = value.get_as<json::string_t>();
      BOOST_TEST(tmp_str == "42");
      double tmp_dbl = value.get_as<json::float_t>();
      BOOST_TEST(tmp_dbl == 42.0);
   }


   {
      value = 42.5;
      double tmp = value.get<json::float_t>();
      BOOST_TEST(tmp == 42.5);
      std::string tmp_str = value.get_as<json::string_t>();
      BOOST_TEST(tmp_str == "42.5");
      int tmp_int = value.get_as<json::int_t>();
      BOOST_TEST(tmp_int == 42);
   }


   {
      value = false;
      bool tmp = value.get_as<json::bool_t>();
      BOOST_TEST(tmp == false);
      int tmp_int = value.get_as<int>();
      BOOST_TEST(tmp_int == 0);
   }

   {
      value = "true";
      BOOST_TEST(true == value.get_as<json::bool_t>());
   }

   {
      value = 42;
      BOOST_TEST(true == value.get_as<json::bool_t>());
      value = 0;
      BOOST_TEST(false == value.get_as<json::bool_t>());
   }

   {
      value = "42";
      BOOST_TEST(42 == value.get_as<int>());
   }


   {
      value = json::null_t();
   }


   return boost::report_errors();
}

