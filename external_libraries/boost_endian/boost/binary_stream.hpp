//  boost/binary_stream.hpp  ----------------------------------------------------------//

//  Copyright Beman Dawes 2009

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See documentation at http://www.boost.org/libs/utility

#ifndef BOOST_BINARY_STREAM_HPP
#define BOOST_BINARY_STREAM_HPP

#include <boost/config.hpp>
#include <ostream>
#include <istream>
#include <string>
#include <cstring>  // for strlen

#ifndef BOOST_NO_CWCHAR
# include <cwchar>   // for wcslen
#endif

//  unformatted binary (as opposed to formatted character-set) input and output

//  Caution: Use only on streams opened with filemode std::ios_base::binary. Thus
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

  //  built-in types  ------------------------------------------------------------------//

  //  omission of bool and void* is deliberate; any semantics would be questionable

  inline std::ostream& operator<=(std::ostream& os, short v)
    { return os.write( reinterpret_cast<const char*>(&v), sizeof(v) ); }
  inline std::istream& operator>=(std::istream& is, short& v)
    { return is.read( reinterpret_cast<char*>(&v), sizeof(v) ); }

  inline std::ostream& operator<=(std::ostream& os, unsigned short v)
    { return os.write( reinterpret_cast<const char*>(&v), sizeof(v) ); }
  inline std::istream& operator>=(std::istream& is, unsigned short& v)
    { return is.read( reinterpret_cast<char*>(&v), sizeof(v) ); }

  inline std::ostream& operator<=(std::ostream& os, int v)
    { return os.write( reinterpret_cast<const char*>(&v), sizeof(v) ); }
  inline std::istream& operator>=(std::istream& is, int& v)
    { return is.read( reinterpret_cast<char*>(&v), sizeof(v) ); }

  inline std::ostream& operator<=(std::ostream& os, unsigned int v)
    { return os.write( reinterpret_cast<const char*>(&v), sizeof(v) ); }
  inline std::istream& operator>=(std::istream& is, unsigned int& v)
    { return is.read( reinterpret_cast<char*>(&v), sizeof(v) ); }

  inline std::ostream& operator<=(std::ostream& os, long v)
    { return os.write( reinterpret_cast<const char*>(&v), sizeof(v) ); }
  inline std::istream& operator>=(std::istream& is, long& v)
    { return is.read( reinterpret_cast<char*>(&v), sizeof(v) ); }

  inline std::ostream& operator<=(std::ostream& os, unsigned long v)
    { return os.write( reinterpret_cast<const char*>(&v), sizeof(v) ); }
  inline std::istream& operator>=(std::istream& is, unsigned long& v)
    { return is.read( reinterpret_cast<char*>(&v), sizeof(v) ); }

  inline std::ostream& operator<=(std::ostream& os, long long v)
    { return os.write( reinterpret_cast<const char*>(&v), sizeof(v) ); }
  inline std::istream& operator>=(std::istream& is, long long& v)
    { return is.read( reinterpret_cast<char*>(&v), sizeof(v) ); }

  inline std::ostream& operator<=(std::ostream& os, unsigned long long v)
    { return os.write( reinterpret_cast<const char*>(&v), sizeof(v) ); }
  inline std::istream& operator>=(std::istream& is, unsigned long long& v)
    { return is.read( reinterpret_cast<char*>(&v), sizeof(v) ); }

  inline std::ostream& operator<=(std::ostream& os, float v)
    { return os.write( reinterpret_cast<const char*>(&v), sizeof(v) ); }
  inline std::istream& operator>=(std::istream& is, float& v)
    { return is.read( reinterpret_cast<char*>(&v), sizeof(v) ); }

  inline std::ostream& operator<=(std::ostream& os, double v)
    { return os.write( reinterpret_cast<const char*>(&v), sizeof(v) ); }
  inline std::istream& operator>=(std::istream& is, double& v)
    { return is.read( reinterpret_cast<char*>(&v), sizeof(v) ); }

  inline std::ostream& operator<=(std::ostream& os, long double v)
    { return os.write( reinterpret_cast<const char*>(&v), sizeof(v) ); }
  inline std::istream& operator>=(std::istream& is, long double& v)
    { return is.read( reinterpret_cast<char*>(&v), sizeof(v) ); }

  inline std::ostream& operator<=(std::ostream& os, char c)
    { return os.put( c ); }
  inline std::istream& operator>=(std::istream& is, char& c)
    { return is.get( c ); }

  inline std::ostream& operator<=(std::ostream& os, signed char c)
    { return os.put( c ); }
  inline std::istream& operator>=(std::istream& is, signed char& c)
    { return is.get( reinterpret_cast<char&>(c) ); }

  inline std::ostream& operator<=(std::ostream& os, unsigned char c)
    { return os.put( c ); }
  inline std::istream& operator>=(std::istream& is, unsigned char& c)
    { return is.get( reinterpret_cast<char&>(c) ); }

  inline std::ostream& operator<=(std::ostream& os, wchar_t v)
    { return os.write( reinterpret_cast<const char*>(&v), sizeof(v) ); }
  inline std::istream& operator>=(std::istream& is, wchar_t& v)
    { return is.read( reinterpret_cast<char*>(&v), sizeof(v) ); }

  //  strings  -------------------------------------------------------------------------//

  inline std::ostream& operator<=(std::ostream& os, const char* p)
    { return os.write( p, std::strlen(p)+1 ); }

  inline std::ostream& operator<=(std::ostream& os, const signed char* p)
    { return os.write( reinterpret_cast<const char*>(p), std::strlen(reinterpret_cast<const char*>(p))+1 ); }

  inline std::ostream& operator<=(std::ostream& os, const unsigned char* p)
    { return os.write( reinterpret_cast<const char*>(p), std::strlen(reinterpret_cast<const char*>(p))+1 ); }

#ifndef BOOST_NO_CWCHAR
  inline std::ostream& operator<=(std::ostream& os, const wchar_t* p)
    { return os.write( reinterpret_cast<const char*>(p), (std::wcslen(p)+1)*sizeof(wchar_t) ); }
#endif

  //  Caution: note the asymmetry between output and input; a string with embedded
  //  nulls will be output with the embedded nulls, but input will stop at the first null.
  //  So it probably isn't a good idea to use these functions for strings with nulls.
  inline std::ostream& operator<=(std::ostream& os, const std::string& s)
    { return os.write( s.c_str(), s.size()+1 ); }
  inline std::istream& operator>=(std::istream& is, std::string& s)
    { return getline(is, s, '\0'); }

#ifndef BOOST_NO_STD_WSTRING
  inline std::ostream& operator<=(std::ostream& os, const std::wstring& s)
    { return os.write( reinterpret_cast<const char*>(s.c_str()), (s.size()+1)*sizeof(wchar_t) ); }
  // TODO: provide input function
#endif

}  // namespace boost

#endif  // BOOST_BINARY_STREAM_HPP
