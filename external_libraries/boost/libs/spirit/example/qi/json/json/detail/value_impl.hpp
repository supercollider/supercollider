/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 *   Copyright (C) 2012 Jeroen Habraken 
 *   Copyright (c) 2011 Joel de Guzman
 *   Copyright (C) 2011, 2012 Object Modeling Designs
 *
 *   Distributed under the Boost Software License, Version 1.0. (See accompanying
 *   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef CIERE_JSON_VALUE_IMPL_HPP
#define CIERE_JSON_VALUE_IMPL_HPP

#include <iostream> //test

#include "../value.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/assert.hpp>

namespace ciere { namespace json
{
   namespace detail
   {
      template< typename T >
      struct extract
      {
         template<typename A>
         static T get(A & v)               { return boost::get<T>(v); }

         template<typename A>
         static const T get(const A & v)   { return boost::get<T>(v); }
      };


      template<typename R>
      struct convert
      {
         BOOST_MPL_ASSERT_MSG(
              !(boost::is_same<R,null_t>::value)
            , CANNOT_GET_AS_WITH_NULL_T
            , (R) 
            );

         template<typename T>
         static R apply( T const & v
                       , typename boost::enable_if<boost::is_convertible<R,T> >::type* dummy=0 )
         {
            return v;
         }


         static R apply( string_t const & v )
         {
            return boost::lexical_cast<R>(v);
         }

         template<typename T>
         static R apply( T const & v
                       , typename boost::disable_if<boost::is_convertible<R,T> >::type* dummy=0 )
         {
            throw get_as_error();
            return R();
         }
      };

      template<>
      struct convert<string_t>
      {
         static string_t apply( string_t const & v)
         {
            return v;
         }

         static string_t apply( float_t const & v )
         {
            return boost::lexical_cast<std::string>(v);
         }

         static string_t apply( int_t const & v )
         {
            return boost::lexical_cast<std::string>(v);
         }

         static string_t apply( bool_t const & v)
         {
            return (v ? "true" : "false");
         }

         static string_t apply( null_t const & )
         {
            return "null";
         }

         template<typename T>
         static string_t apply( T const & v )
         {
            throw get_as_error();
            return "";
         }
      };


      template<>
      struct convert<bool_t>
      {
         template<typename T>
         static bool_t apply( T const & v
                            , typename boost::enable_if<boost::is_convertible<bool_t,T> >::type* dummy=0 )
         {
            return v;
         }

         static bool_t apply( string_t const & v )
         {
            if( v == "true" ) return true;
            else              return false;
         }

         template<typename T>
         static bool_t apply( T const & v 
                            , typename boost::disable_if<boost::is_convertible<bool_t,T> >::type* dummy=0 )
         {
            throw get_as_error();
            return false;
         }
      };


      
      template<typename T>
      struct convert_to : public boost::static_visitor<T>
      {
         template<typename V>
         T operator()(V const & v) const
         {
            try
            {
               return convert<T>::apply(v);
            }
            catch(...)
            {
               throw get_as_error();
            }
         }

         // if the types are the same, no conversion required
         T operator()(T const & v) const
         {
            return v;
         }
      };
   }


   struct value::make_json_value
   {
      json::value&       operator()(json::value& v)       const { return v; }
      const json::value& operator()(const json::value& v) const { return v; }
   };

   struct value::make_json_member
   {
      value::member       operator()(object_t::value_type & v)       const  { return value::member(v);       }
      value::const_member operator()(const object_t::value_type & v) const  { return value::const_member(v); }
   };


   // -------------------------------------------------------------------------------
   //    array handling
   // -------------------------------------------------------------------------------

   /**
    *  Add compatible type to the end of the array
    */
   template< typename T >
   value& value::add( T v )
   {
      push_back(v);
      return *this;
   }


   /**
    *  Add a compatible type to the end of the array, functor style.
    */
   template< typename T >
   value& value::operator()( T v )
   {
      return add(v);
   } 

   /**
    *  Add compatible type to the end of the array, stl style-ish
    *  Actually returns a reference the newly added value.
    */
   template< typename T >
   value& value::push_back( T v )
   {
      array_t* p_array = boost::get<array_t>(&base_type::get());
      
      // if we aren't an array, we need to be an array
      if( !p_array )
      {
         base_type::get() = array_t();
         p_array = boost::get<array_t>(&base_type::get());
      }

      p_array->push_back( (json::value(v)) );
      return p_array->back();
   } 


   // -------------------------------------------------------------------------------
   //    object handling
   // -------------------------------------------------------------------------------

   template< typename T >
   value& value::set( string_t const & name, T v )
   {
      object_t* p_object = boost::get<object_t>(&base_type::get());

      // if this isn't an object type ... it needs to be
      if( !p_object )
      {
         base_type::get() = object_t();
         p_object = boost::get<object_t>(&base_type::get());
      }

      (*p_object)[name] = v;
      return *this;
   }


   template< typename T >
   value& value::operator()( string_t const & name, T v )
   {
      return set(name,v);
   }


   // -------------------------------------------------------------------------------
   // Extract based on type
   // -------------------------------------------------------------------------------
   template< typename T >
   T value::get()
   {
      return detail::extract<T>::get(base_type::get());
   }

   template< typename T >
   const T value::get() const
   {
      return detail::extract<T>::get(base_type::get());
   }
   // -------------------------------------------------------------------------------
   // -------------------------------------------------------------------------------

   // -------------------------------------------------------------------------------
   // Extract based on type and convert to requested type
   // -------------------------------------------------------------------------------
   template< typename T >
   T value::get_as()
   {
      return boost::apply_visitor(detail::convert_to<T>(),base_type::get());
   }

   template< typename T >
   const T value::get_as() const
   {
      return boost::apply_visitor(detail::convert_to<T>(),base_type::get());
   }
   // -------------------------------------------------------------------------------
   // -------------------------------------------------------------------------------

}}

#endif // CIERE_JSON_VALUE_IMPL_HPP
