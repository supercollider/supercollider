//  boost/integer/endian_binary_stream.hpp  --------------------------------------------//

//  Copyright Beman Dawes 2009

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See library home page at http://www.boost.org/libs/endian

#ifndef BOOST_ENDIAN_BINARY_STREAM_HPP
#define BOOST_ENDIAN_BINARY_STREAM_HPP

#include <boost/integer/endian.hpp>
#include <boost/utility/enable_if.hpp>
#include <ostream>
#include <istream>

//  unformatted binary (as opposed to formatted character) stream insertion
//  and extraction.

//  Warning: Use only on streams opened with filemode std::ios_base::binary. Thus
//  unformatted binary I/O should not be with the standard streams (cout, cin, etc.)
//  since they are opened in text mode. Use on text streams may produce incorrect
//  results, such as insertion of unwanted characters or premature end-of-file.
//  For example, on Windows 0x0D would become 0x0D, 0x0A.

//  Caution: When mixing formatted (i.e. operator << or >>) and unformatted (i.e.
//  operator <= or >=) be aware that << and >> take precedence over <= and >=. Use
//  parentheses to force correct order of evaluation. For example:
//
//          my_stream << foo <= bar;    // no parentheses needed
//          (my_stream <= foo) << bar;  // parentheses required 

//  This implementation uses reinterpret_cast<>() when needed to convert one pointer
//  type to another. See 5.2.10 [expr.reinterpret.cast], Reinterpret cast, para 7.

namespace boost
{
  namespace integer
  {
     template< class T > struct is_endian { static const bool value = false; };

     template<> struct is_endian<big8_t> { static const bool value = true; };
     template<> struct is_endian<big16_t> { static const bool value = true; };
     template<> struct is_endian<big24_t> { static const bool value = true; };
     template<> struct is_endian<big32_t> { static const bool value = true; };
     template<> struct is_endian<big40_t> { static const bool value = true; };
     template<> struct is_endian<big48_t> { static const bool value = true; };
     template<> struct is_endian<big56_t> { static const bool value = true; };
     template<> struct is_endian<big64_t> { static const bool value = true; };

     template<> struct is_endian<ubig8_t> { static const bool value = true; };
     template<> struct is_endian<ubig16_t> { static const bool value = true; };
     template<> struct is_endian<ubig24_t> { static const bool value = true; };
     template<> struct is_endian<ubig32_t> { static const bool value = true; };
     template<> struct is_endian<ubig40_t> { static const bool value = true; };
     template<> struct is_endian<ubig48_t> { static const bool value = true; };
     template<> struct is_endian<ubig56_t> { static const bool value = true; };
     template<> struct is_endian<ubig64_t> { static const bool value = true; };

     template<> struct is_endian<little8_t> { static const bool value = true; };
     template<> struct is_endian<little16_t> { static const bool value = true; };
     template<> struct is_endian<little24_t> { static const bool value = true; };
     template<> struct is_endian<little32_t> { static const bool value = true; };
     template<> struct is_endian<little40_t> { static const bool value = true; };
     template<> struct is_endian<little48_t> { static const bool value = true; };
     template<> struct is_endian<little56_t> { static const bool value = true; };
     template<> struct is_endian<little64_t> { static const bool value = true; };

     template<> struct is_endian<ulittle8_t> { static const bool value = true; };
     template<> struct is_endian<ulittle16_t> { static const bool value = true; };
     template<> struct is_endian<ulittle24_t> { static const bool value = true; };
     template<> struct is_endian<ulittle32_t> { static const bool value = true; };
     template<> struct is_endian<ulittle40_t> { static const bool value = true; };
     template<> struct is_endian<ulittle48_t> { static const bool value = true; };
     template<> struct is_endian<ulittle56_t> { static const bool value = true; };
     template<> struct is_endian<ulittle64_t> { static const bool value = true; };

     template<> struct is_endian<native8_t> { static const bool value = true; };
     template<> struct is_endian<native16_t> { static const bool value = true; };
     template<> struct is_endian<native24_t> { static const bool value = true; };
     template<> struct is_endian<native32_t> { static const bool value = true; };
     template<> struct is_endian<native40_t> { static const bool value = true; };
     template<> struct is_endian<native48_t> { static const bool value = true; };
     template<> struct is_endian<native56_t> { static const bool value = true; };
     template<> struct is_endian<native64_t> { static const bool value = true; };

     template<> struct is_endian<unative8_t> { static const bool value = true; };
     template<> struct is_endian<unative16_t> { static const bool value = true; };
     template<> struct is_endian<unative24_t> { static const bool value = true; };
     template<> struct is_endian<unative32_t> { static const bool value = true; };
     template<> struct is_endian<unative40_t> { static const bool value = true; };
     template<> struct is_endian<unative48_t> { static const bool value = true; };
     template<> struct is_endian<unative56_t> { static const bool value = true; };
     template<> struct is_endian<unative64_t> { static const bool value = true; };
    
 # if defined(BOOST_HAS_INT16_T)
     template<> struct is_endian<aligned_big16_t> { static const bool value = true; };
     template<> struct is_endian<aligned_ubig16_t> { static const bool value = true; };
     template<> struct is_endian<aligned_little16_t> { static const bool value = true; };
     template<> struct is_endian<aligned_ulittle16_t> { static const bool value = true; };
# endif

# if defined(BOOST_HAS_INT32_T)
     template<> struct is_endian<aligned_big32_t> { static const bool value = true; };
     template<> struct is_endian<aligned_ubig32_t> { static const bool value = true; };
     template<> struct is_endian<aligned_little32_t> { static const bool value = true; };
     template<> struct is_endian<aligned_ulittle32_t> { static const bool value = true; };
# endif

# if defined(BOOST_HAS_INT64_T)
     template<> struct is_endian<aligned_big64_t> { static const bool value = true; };
     template<> struct is_endian<aligned_ubig64_t> { static const bool value = true; };
     template<> struct is_endian<aligned_little64_t> { static const bool value = true; };
     template<> struct is_endian<aligned_ulittle64_t> { static const bool value = true; };
# endif

     template < class Endian >
       inline typename boost::enable_if< is_endian<Endian>, std::ostream & >::type
         operator<=( std::ostream & os, const Endian & e )
     {
       return os.write( reinterpret_cast<const char*>(&e), sizeof(e) );
     }

     template < class Endian >
       inline typename boost::enable_if< is_endian<Endian>, std::istream & >::type
         operator>=( std::istream & is, Endian & e )
     {
       return is.read( reinterpret_cast<char*>(&e), sizeof(e) );
     }

  }  // namespace integer
}  // namespace boost

#endif  // BOOST_ENDIAN_BINARY_STREAM_HPP
