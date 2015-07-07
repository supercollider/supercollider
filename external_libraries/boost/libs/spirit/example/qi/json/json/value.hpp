/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 *   Copyright (C) 2012 Jeroen Habraken
 *   Copyright (C) 2011, 2012 Object Modeling Designs
 *
 *   Distributed under the Boost Software License, Version 1.0. (See accompanying
 *   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef CIERE_JSON_VALUE_HPP
#define CIERE_JSON_VALUE_HPP

#include <string>
#include <boost/variant.hpp>
#include <boost/exception/all.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/container/stable_vector.hpp>
#include <boost/spirit/include/support_extended_variant.hpp>
#include <boost/mpl/or.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/utility/enable_if.hpp>


namespace ciere { namespace json
{

   // ---------------------------------------------------
   // exceptions
   // ---------------------------------------------------

   struct not_container_type : virtual boost::exception, virtual std::exception {};
   struct not_object_type    : virtual boost::exception, virtual std::exception {};
   struct not_array_type     : virtual boost::exception, virtual std::exception {};
   struct not_number_type    : virtual boost::exception, virtual std::exception {};
   struct get_as_error       : virtual boost::exception, virtual std::exception {};

   // ---------------------------------------------------
   // ---------------------------------------------------


   // ------------------- ast types --------------------
   //
   typedef std::string                                  string_t;
   typedef double                                       double_t;
   typedef double                                       float_t;
   typedef int64_t                                      int_t;
   typedef bool                                         bool_t;
   struct                                               null_t
   {};
   class                                                value;
   typedef std::map<std::string, value>                 object_t;
   typedef std::pair<std::string, value>                object_member_t;
   typedef boost::container::stable_vector<value>       array_t;


   // nulls always compare
   inline bool operator==(null_t,null_t) { return true;  }
   inline bool operator!=(null_t,null_t) { return false; }

   inline std::ostream & operator<<(std::ostream & out, null_t)
   { out << "null"; return out; }



   /**
    *  possible types being stored in the json::value
    */
   enum value_types
   {
      string_type,
      double_type,
      int_type,
      bool_type,
      null_type,
      value_type,
      object_type,
      array_type
   };


   /**
    *
    *
    */
   class value  
      : public boost::spirit::extended_variant<
                   null_t
                 , bool_t
                 , string_t
                 , int_t
                 , double_t
                 , object_t
                 , array_t      >
   {
      public:
         // -------------------------------------------------------------------------------
         // -------------------------------------------------------------------------------
         value(null_t val = null_t())     : base_type(val) {}
         value(bool_t val)                : base_type(val) {}
         value(string_t const & val)      : base_type(val) {}
         value(char const * val)          : base_type((string_t(val))) {}
         value(object_t const & val)      : base_type(val) {}
         value(array_t const & val)       : base_type(val) {}
         value(value const & rhs)         : base_type(rhs.get_ast()) {}

         template< typename T >
         value(T val,
               typename boost::enable_if<boost::is_floating_point<T> >::type* = 0)
            : base_type( (double_t(val)) ) {}


         template< typename T >
         value(T val,
               typename boost::enable_if<
                   boost::mpl::or_<
                        boost::is_integral<T>
                      , boost::is_enum<T>
                      >
                  >::type* = 0)
            : base_type( (int_t(val)) ) {}

         // -------------------------------------------------------------------------------
         // -------------------------------------------------------------------------------

         /**
          *  Returns the type currently being stored
          */
         value_types type() const;


         // -------------------------------------------------------------------------------
         //    iterator helpers
         // -------------------------------------------------------------------------------
         /**
          *  type returned from an object iterator
          */
         struct member
         {
            member(object_t::value_type & m) : member_pair(m) {}
            
            std::string         name()  const { return member_pair.first; }
            json::value &       value()       { return member_pair.second; }
            json::value const & value() const { return member_pair.second; }

            private:
               object_t::value_type & member_pair;
         };

         struct const_member
         {
            const_member(object_t::value_type & m)       : member_pair(m) {}
            const_member(object_t::value_type const & m) : member_pair(m) {}

            std::string         name()  const { return member_pair.first; }
            const json::value & value() const { return member_pair.second; }

            private:
               object_t::value_type const & member_pair;
         };


         // -------------------------------------------------------------------------------
         //    iterator types
         // -------------------------------------------------------------------------------

      private:
         struct make_json_value;
         struct make_json_member;

      public:
         typedef boost::transform_iterator<  make_json_member
                                           , object_t::iterator
                                           , member >                      object_iterator;

         typedef boost::transform_iterator<  make_json_member
                                           , object_t::const_iterator
                                           , const_member >                const_object_iterator;

         typedef boost::transform_iterator<  make_json_value
                                           , array_t::iterator
                                           , value & >                     array_iterator;

         typedef boost::transform_iterator<  make_json_value
                                           , array_t::const_iterator
                                           , value const & >               const_array_iterator;


      

         // -------------------------------------------------------------------------------
         //    iterator access
         // -------------------------------------------------------------------------------

         /**
          *  Returns an iterator pointing to the fist member in the object if the value type
          *  is object_type. Otherwise, throw not_object_type.
          */
         object_iterator       begin_object();
         const_object_iterator begin_object() const;

         /**
          *  Returns an iterator that can be compared to determine the end of members in an object. If
          *  the value type is not an object then throw not_object_type.
          */
         object_iterator       end_object();
         const_object_iterator end_object() const;

         /**
          *  Returns an iterator pointing to the fist member in the array if the value type
          *  is array_type. Otherwise, throw not_array_type.
          */
         array_iterator       begin_array();
         const_array_iterator begin_array() const;

         /**
          *  Returns an iterator that can be compared to determine the end of members in an array. If the 
          *  value type is not array_type throw not_array_type.
          */
         array_iterator       end_array();
         const_array_iterator end_array() const;

         // -------------------------------------------------------------------------------
         // -------------------------------------------------------------------------------



         // -------------------------------------------------------------------------------
         //    array handling
         // -------------------------------------------------------------------------------

         /**
          *  Add compatible type to the end of the array
          */
         template< typename T >
         value & add(T v);
         value & add(char const * v);

         /**
          *  Add a compatible type to the end of the array, functor style.
          */
         template< typename T >
         value & operator()(T v);

         /**
          *  Add compatible type to the end of the array, stl style-ish
          *  Actually returns a reference the newly added value.
          */
         template< typename T >
         value & push_back(T v);
         value & push_back(char const * v);

         /**
          *  Access array element stored at index value. Will throw out_of_range exception
          *  if index is not a valid index for the container.
          */
         value &       at(size_t index);
         value const & at(size_t index) const;

         /**
          *  Access array element stored at index value. If the array length is less than
          *  index, the array will be expanded to index + 1 length and additional
          *  locations will be filled with null_type.
          */
         value &       operator[](int index);
         value const & operator[](int index) const;

         /**
          *  Erase removes the value at index. 
          */
         value & erase(int index);


         /**
          *  Returns the number of elements stored if the value is holding an array or
          *  object type. If it is not an array or object type the method throws
          *  ciere::json::not_array_type
          */
         std::size_t length() const;


         // -------------------------------------------------------------------------------
         // -------------------------------------------------------------------------------


         // -------------------------------------------------------------------------------
         //    object handling
         // -------------------------------------------------------------------------------

         /**
          *  Set the member name to value v.
          *  If member name already exists, its value will be replaced.
          *
          *  If the value object is not an object type it will become one.
          */
         template< typename T >
         value & set(string_t const & name, T v);

         // template can be ambigous for some types ... lets get specific
         value & set(string_t const & name, char const * v);

         /**
          *  Functor style setting. The first value is the name of the member to add.
          *  the second value is the value to be set.
          */
         template< typename T >
         value & operator()(string_t const & name, T v);

         /**
          *  Tests if the object contains the specified named object.
          */
         bool has_key(std::string const & key) const;

         /**
          *  Access the object's member with the specified name.
          *  If no member exists, one will be created with a default value
          *  of null_t.
          */
         value &       operator[](char const * v);
         value &       operator[](string_t const & v);
         value const & operator[](char const * v) const;
         value const & operator[](string_t const & v) const;

         value & erase(char const * v){ return erase( (string_t(v)) ); }
         value & erase(string_t const & v);

         // -------------------------------------------------------------------------------
         // -------------------------------------------------------------------------------


         /**
          *  Extract the actual value based on the type T. If the json::value
          *  does not contain type T, the method will throw.
          */
         template< typename T >
         T get();

         template< typename T >
         const T get() const;

         /**
          *  Extract the contained value as if it is of type T. If the
          *  contained type is not of type T, the value will be converted
          *  if it can. If no conversion can be found, it will throw.
          */
         template< typename T >
         T get_as();

         template< typename T >
         const T get_as() const;


         // -------------------------------------------------------------------------------
         //    conversion
         // -------------------------------------------------------------------------------

         operator string_t() const;

         // -------------------------------------------------------------------------------
         // -------------------------------------------------------------------------------


         base_type::variant_type       & get_ast()       { return base_type::get(); }
         base_type::variant_type const & get_ast() const { return base_type::get(); }
   };


   /**
    *  Constructs a value containing an array type
    */
   value array();

   /**
    *  Constructs a value containing an object type
    */
   value object();

   bool operator==(value const & a, value const & b);
   bool operator!=(value const & a, value const & b);
   bool operator<(value const & a, value const & b);

}}

#include "detail/value_impl.hpp"


#endif  // CIERE_JSON_VALUE_HPP
