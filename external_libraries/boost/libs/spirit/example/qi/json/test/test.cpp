/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 *   Copyright (C) 2010, 2011 Object Modeling Designs
 */

#include <boost/detail/lightweight_test.hpp>
#include "json/value.hpp"
#include "json/io.hpp"
#include "json/parser/grammar.hpp"
#include "json/parser/grammar_def.hpp"


int main()
{
   boost::spirit::ascii::space_type space;

   {
      typedef std::string::const_iterator iter_t;
      typedef ciere::json::parser::grammar<iter_t> grammar_t;

      std::string test_input(
         "{"
         "  \"value\"  : 12.34,"
         "  \"on\"     : false,"
         "  \"jump\"   : null,"
         "  \"result\" : [1,34.5,{},\"Sosa did fine.\\u263A\",\"Snowman: \\u2603\"]"
         "}"
         );

      grammar_t grammar;
      ciere::json::value value;

      std::cout << "test input: " << test_input << std::endl;

      iter_t iter = test_input.begin();
      iter_t end =  test_input.end();

      BOOST_TEST( boost::spirit::qi::phrase_parse( iter, end, grammar, space, value ) );
      BOOST_TEST( iter == end );

      std::cout << "ast : " << value << std::endl;
   }

   {
      typedef std::string::const_iterator iter_t;
      typedef ciere::json::parser::grammar<iter_t> grammar_t;

      std::string test_input(
         "{"
         "  \"foo\":123.456,"
         "  \"the_value\":        42,"
         "  \"bar\":[1,\"some test \", {\"on\":false,\"object\":null}],"
         "  \"gorp\":\"how about this mom\""
         "}"
         );

      grammar_t grammar;
      ciere::json::value value;

      std::cout << "test input: " << test_input << std::endl;

      iter_t iter = test_input.begin();
      iter_t end =  test_input.end();

      BOOST_TEST( boost::spirit::qi::phrase_parse( iter, end, grammar, space, value ) );
      BOOST_TEST( iter == end );

      std::cout << "ast : " << value << std::endl;
      value["bar"].erase(1);
      std::cout << "ast post erase : " << value << std::endl;
      value.erase("bar");
      std::cout << "ast post erase : " << value << std::endl;
   }

   return boost::report_errors();
}

