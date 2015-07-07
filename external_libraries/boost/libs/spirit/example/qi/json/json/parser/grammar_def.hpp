/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 *   Copyright (c) 2011 Joel de Guzman
 *   Copyright (C) 2010, 2011 Object Modeling Designs
 *
 *   Distributed under the Boost Software License, Version 1.0. (See accompanying
 *   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef CIERE_JSON_DETAIL_GRAMMAR_DEF_HPP
#define CIERE_JSON_DETAIL_GRAMMAR_DEF_HPP

#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS

#include <string>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include "grammar.hpp"


namespace ciere { namespace json { namespace parser
{

   namespace qi = boost::spirit::qi;
   namespace ascii = boost::spirit::ascii;
   namespace phoenix = boost::phoenix;

   typedef boost::uint32_t uchar; // a unicode code point

   namespace detail
   {
      struct push_utf8
      {
         template <typename S, typename C>
         struct result { typedef void type; };

         void operator()(std::string& utf8, uchar code_point) const
         {
            typedef std::back_insert_iterator<std::string> insert_iter;
            insert_iter out_iter(utf8);
            boost::utf8_output_iterator<insert_iter> utf8_iter(out_iter);
            *utf8_iter++ = code_point;
         }
      };

      struct push_esc
      {
         template <typename S, typename C>
         struct result { typedef void type; };

         void operator()(std::string& utf8, uchar c) const
         {
            switch (c)
            {
               case ' ': utf8 += ' ';          break;
               case '\t': utf8 += '\t';        break;
               case '0': utf8 += char(0);      break;
               case 'a': utf8 += 0x7;          break;
               case 'b': utf8 += 0x8;          break;
               case 't': utf8 += 0x9;          break;
               case 'n': utf8 += 0xA;          break;
               case 'v': utf8 += 0xB;          break;
               case 'f': utf8 += 0xC;          break;
               case 'r': utf8 += 0xD;          break;
               case 'e': utf8 += 0x1B;         break;
               case '"': utf8 += '"';          break;
               case '/': utf8 += '/';          break;
               case '\\': utf8 += '\\';        break;

               case '_': push_utf8()(utf8, 0xA0);  break;
               case 'N': push_utf8()(utf8, 0x85);  break;
               case 'L': push_utf8()(utf8, 0x2028);  break;
               case 'P': push_utf8()(utf8, 0x2029);  break;
            }
         }
      };


      template< typename Iterator >
      unicode_string<Iterator>::unicode_string()
         : unicode_string::base_type(double_quoted)
      {
         qi::char_type char_;
         qi::_val_type _val;
         qi::_r1_type _r1;
         qi::_1_type _1;
         qi::_2_type _2;
         qi::lit_type lit;
         qi::eol_type eol;
         qi::repeat_type repeat;
         qi::hex_type hex;

         using boost::spirit::qi::uint_parser;
         using boost::phoenix::function;
         using boost::phoenix::ref;

         uint_parser<uchar, 16, 4, 4> hex4;
         uint_parser<uchar, 16, 8, 8> hex8;
         function<detail::push_utf8> push_utf8;
         function<detail::push_esc> push_esc;

         escape =
                ('x' > hex)                     [push_utf8(_r1, _1)]
            |   ('u' > hex4)                    [push_utf8(_r1, _1)]
            |   ('U' > hex8)                    [push_utf8(_r1, _1)]
            |   char_("0abtnvfre\"/\\N_LP \t")  [push_esc(_r1, _1)]
            |   eol                             // continue to next line
            ;

         char_esc =
            '\\' > escape(_r1)
            ;

         double_quoted =
              '"'
            > *(char_esc(_val) | (~char_('"'))    [_val += _1])
            > '"'
            ;
      }

   }  // end detail


   template< typename Iterator >
   grammar<Iterator>::grammar()
      : grammar::base_type( value )
   {
      qi::_val_type            _val;
      qi::lit_type             lit;
      ascii::char_type         char_;
      qi::double_type          double_;
      qi::int_parser<int64_t>  int_;
      qi::bool_type            bool_value;
      qi::attr_type            attr;
      qi::lexeme_type          lexeme;

      value =
           null_value
         | bool_value
         | string_value
         | lexeme[int_ >> !char_(".eE")]
         | double_
         | object
         | array
         ;

      object =
            lit('{')
         >> -(member_pair % ',')
         >> lit('}')
         ;

      member_pair =
            string_value
         >> ':'
         >> value
         ;

      array = 
            lit('[')
         >> -(value % ',')
         >> lit(']')
         ;

      null_value = 
            lit("null") 
         >> attr(json::null_t())
         ;
   }

}}}


#endif  // CIERE_JSON_DETAIL_GRAMMAR_DEF_HPP
