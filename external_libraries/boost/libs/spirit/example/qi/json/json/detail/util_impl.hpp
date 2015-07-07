/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 *   Copyright (C) 2011, 2012 Object Modeling Designs
 *
 *   Distributed under the Boost Software License, Version 1.0. (See accompanying
 *   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 *
 */

#ifndef CIERE_JSON_UTIL_IMPL_HPP
#define CIERE_JSON_UTIL_IMPL_HPP

#include "../util.hpp"

namespace ciere { namespace json
{
   namespace detail
   {
   }

   /**
    *  Returns true if elements of m match those value.
    */
   bool match( value const & value, value const & m )
   {
      return false;
   }

   /**
    *  Results in : target = target + value
    */
   bool combine( value& target, value const & value )
   {
      return false;
   }

}}

#endif // CIERE_JSON_UTIL_IMPL_HPP
