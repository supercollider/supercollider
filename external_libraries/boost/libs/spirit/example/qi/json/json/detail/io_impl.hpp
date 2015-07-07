/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 *   Copyright (C) 2011, 2012 Object Modeling Designs
 *
 *   Distributed under the Boost Software License, Version 1.0. (See accompanying
 *   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 *
 */

#ifndef CIERE_JSON_IO_IMPL_HPP
#define CIERE_JSON_IO_IMPL_HPP

#include <string>
#include <fstream>
#include <istream>
#include <ios>
#include <boost/foreach.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

#include "../io.hpp"
#include "../parser/grammar.hpp"

namespace ciere { namespace json
{
   namespace spirit = boost::spirit;

   namespace detail
   {
      struct printer : public boost::static_visitor<>
      {
         printer(std::ostream& s) : stream(s) {}

         void operator()(string_t const & utf) const
         {
            stream << '"';

            typedef ::boost::uint32_t ucs4_char;
            typedef boost::u8_to_u32_iterator<std::string::const_iterator> iter_t;
            iter_t f = utf.begin();
            iter_t l = utf.end();

            for (iter_t i = f; i != l; ++i)
            {
               ucs4_char c = *i;
               switch (c)
               {
                  case 0:       stream << "\\0";  break;
                  case 0x7:     stream << "\\a";  break;
                  case 0x8:     stream << "\\b";  break;
                  case 0x9:     stream << "\\t";  break;
                  case 0xA:     stream << "\\n";  break;
                  case 0xB:     stream << "\\v";  break;
                  case 0xC:     stream << "\\f";  break;
                  case 0xD:     stream << "\\r";  break;
                  case 0x1B:    stream << "\\e";  break;
                  case '"':     stream << "\\\""; break;
                  case '\\':    stream << "\\\\"; break;
                  case 0xA0:    stream << "\\_";  break;
                  case 0x85:    stream << "\\N";  break;
                  case 0x2028:  stream << "\\L";  break;
                  case 0x2029:  stream << "\\P";  break;
                  default:      stream << boost::spirit::to_utf8(c);
               }
            }

            stream << '"';
         }

         template< typename T >
         void operator()(T const & value) const
         {
            stream << value;
         }

         void operator()(double d) const
         {
            // javascript's handling of NaN and +/-Infinity
            // isn't so great. JSON simply follows the javascript
            // standard. We can output nan and infinity; however,
            // we cannot actually parse it back in afaict because
            // the javascript side is generating a null?
            //
            // TODO: clear this up with something definitive
            if(boost::math::isnan(d))
            {
               stream << "NaN";
               return;
            }
            if(boost::math::isinf(d))
            {
               if(d < 0.0) { stream << '-'; }
               stream << "Infinity";
               return;
            }
            stream << d;
         }

         void operator()(bool_t value) const
         {
            stream << (value?"true":"false");
         }

         void operator()(null_t value) const
         {
            stream << "null";
         }

         void operator()(object_t const & obj) const
         {
            stream << "{";
            bool first = true;

            BOOST_FOREACH( object_t::value_type const & v, obj )
            {
               if( first ) { first = false;  }
               else        { stream << ", "; }
               
               stream << '"' << v.first << "\":";
               boost::apply_visitor( *this,v.second);
            }

            stream << "}";
         }

         void operator()(array_t const & arr) const
         {
            stream << "[";
            bool first = true;

            BOOST_FOREACH( value const & v, arr )
            {
               if( first ) { first = false;  }
               else        { stream << ", "; }

               boost::apply_visitor(*this,v);
            }
            stream << "]";
         }

         std::ostream& stream;
      };
   }

   inline std::ostream& operator<<(std::ostream& stream, value const & v)
   {
      boost::apply_visitor(detail::printer(stream),v);
      return stream;
   }

   inline std::istream& operator>>( std::istream& stream, value& object )
   {
      if( !json::read( stream, object ) )
      {
         stream.setstate( std::ios_base::failbit );
      }

      return stream;
   }

   inline bool read( std::istream& stream, value& object)
   {
      typedef parser::grammar< spirit::istream_iterator > grammar_t;

      stream.unsetf( std::ios::skipws );
      spirit::istream_iterator iter( stream );
      spirit::istream_iterator end_iter;
      
      grammar_t grammar;
      return( spirit::qi::phrase_parse( iter, end_iter,
                                        grammar,
                                        spirit::ascii::space_type(),
                                        object ) );
   }

   inline bool read( std::string const & filename, value& object)
   {
      std::ifstream stream( filename.c_str() );
      if( !stream.is_open() )
      {
         return false;
      }

      return read( stream, object );
   }

   inline value construct( std::string const & input )
   {
      typedef std::string::const_iterator iter_t;
      typedef parser::grammar<iter_t> grammar_t;

      grammar_t grammar;
      json::value value;

      iter_t iter = input.begin();
      iter_t end =  input.end();

      spirit::qi::phrase_parse( iter, end,
                                grammar,
                                spirit::ascii::space_type(),
                                value );

      return value;
   }
}}

#endif // CIERE_JSON_IO_IMPL_HPP
