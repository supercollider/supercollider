/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 *   Copyright (C) 2010, 2011 Object Modeling Designs
 *
 *   Distributed under the Boost Software License, Version 1.0. (See accompanying
 *   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef CIERE_JSON_DETAIL_GRAMMAR_HPP
#define CIERE_JSON_DETAIL_GRAMMAR_HPP

#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS

#include <string>
#include <boost/spirit/include/qi.hpp>
#include "../value.hpp"


namespace ciere { namespace json { namespace parser
{
   namespace qi      = boost::spirit::qi;
   namespace ascii   = boost::spirit::ascii;

   namespace detail
   {
      template <typename Iterator>
      struct unicode_string : qi::grammar<Iterator, std::string()>
      {
         unicode_string();
         qi::rule<Iterator, void(std::string&)> escape;
         qi::rule<Iterator, void(std::string&)> char_esc;
         qi::rule<Iterator, std::string()> double_quoted;
      };
   }


   template< typename Iterator >
   struct grammar : qi::grammar< Iterator, json::value(), ascii::space_type >
   {
      grammar();
      qi::rule< Iterator, json::value()           , ascii::space_type > value;
      qi::rule< Iterator, json::object_t()        , ascii::space_type > object;
      qi::rule< Iterator, json::object_member_t() , ascii::space_type > member_pair;
      qi::rule< Iterator, json::array_t()         , ascii::space_type > array;
      detail::unicode_string< Iterator                                > string_value;
      qi::rule< Iterator, json::null_t()          , ascii::space_type > null_value;
   };

}}}

#endif  // CIERE_JSON_DETAIL_GRAMMAR_HPP
