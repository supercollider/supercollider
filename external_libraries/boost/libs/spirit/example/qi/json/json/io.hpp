/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 *   Copyright (C) 2011, 2012 Object Modeling Designs
 *
 *   Distributed under the Boost Software License, Version 1.0. (See accompanying
 *   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 *
 */

#ifndef CIERE_JSON_IO_HPP
#define CIERE_JSON_IO_HPP

#include <iostream>
#include <string>
#include "value.hpp"

namespace ciere { namespace json
{

   std::ostream& operator<<(std::ostream& stream, value const & value);

   std::istream& operator>>( std::istream& stream, value& object );

   /**
    *  Reads from filename and populates object with the contents of the filename.
    */
   bool read( std::string const & filename, value& object);

   /**
    * Read from stream and populate object with json contents
    */
   bool read( std::istream& stream, value& object);

   /**
    * construct a json value from the input string
    */
   value construct( std::string const & input );

}}


#include "detail/io_impl.hpp"

#endif  // CIERE_JSON_IO_HPP
