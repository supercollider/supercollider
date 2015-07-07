/**
 *   Copyright (C) 2012 ciere consulting, ciere.com
 *   Copyright (C) 2011, 2012 Object Modeling Designs
 *
 *   Distributed under the Boost Software License, Version 1.0. (See accompanying
 *   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 *
 */

#include "../json/value.hpp"
#include <boost/variant.hpp>

namespace ciere { namespace json
{
   namespace detail
   {
      struct value_compare
      {
         typedef bool result_type;

         template <typename A, typename B>
         bool operator()(A const& a, B const& b) const
         {
            return false;
         }

         template <typename T>
         bool operator()(T const& a, T const& b) const
         {
            return a < b;
         }

         bool operator()(double_t const& a, int_t const& b) const
         {
            return a < b;
         }

         bool operator()(int_t const& a, double_t const& b) const
         {
            return a < b;
         }

         bool operator()(object_t const& a, object_t const& b)
         {
            return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
         }

         bool operator()(array_t const& a, array_t const& b)
         {
            return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
         }
      };


      struct value_equal
      {
         typedef bool result_type;

         template <typename A, typename B>
         bool operator()(A const& a, B const& b) const
         {
            return false;
         }

         template <typename T>
         bool operator()(T const& a, T const& b) const
         {
            return a == b;
         }

         bool operator()(double_t const& a, int_t const& b) const
         {
            return a == b;
         }

         bool operator()(int_t const& a, double_t const& b) const
         {
            return a == b;
         }

         bool operator()(object_t const& a, object_t const& b)
         {
            if(a.size() != b.size()) { return false; }

            object_t::const_iterator ii = b.begin();
            for( object_t::const_iterator i = a.begin(); i != a.end(); ++i )
            {
               if (*i != *ii++){ return false; }
            }
            return true;
         }

         bool operator()(array_t const& a, array_t const& b)
         {
            if(a.size() != b.size()) { return false; }

            array_t::const_iterator ii = b.begin();
            for( array_t::const_iterator i = a.begin(); i != a.end(); ++i )
            {
               if(*i != *ii++) { return false; }
            }
            return true;
         }
      };


      struct get_type : public boost::static_visitor<value_types>
      {
         value_types operator()(string_t) const { return string_type; }
         value_types operator()(int_t)    const { return int_type;    }
         value_types operator()(double_t) const { return double_type; }
         value_types operator()(bool_t)   const { return bool_type;   }
         value_types operator()(null_t)   const { return null_type;   }
         value_types operator()(object_t) const { return object_type; }
         value_types operator()(array_t)  const { return array_type;  }
      };



      struct get_length : public boost::static_visitor<std::size_t>
      {
         std::size_t operator()(const object_t& v) const { return v.size() ; }
         std::size_t operator()(const array_t& v)  const { return v.size() ; }

         template< typename T >
         std::size_t operator()(const T&) const { boost::throw_exception( (not_container_type()) ); }
      };


      struct has_key_visitor : public boost::static_visitor<bool>
      {
         has_key_visitor(const object_t::key_type & key) : key_(key)  {}

         template< typename T >
         bool operator()(const T&)             const { return false; }

         bool operator()(const object_t & v)   const { return v.find(key_) != v.end(); }

         const object_t::key_type & key_;
      };


   }  // namespace detail


   value_types value::type() const
   {
      return boost::apply_visitor(detail::get_type(),*this);
   }

   std::size_t value::length() const
   {
      return boost::apply_visitor(detail::get_length(),*this);
   }

   // -------------------------------------------------------------------------------
   //    iterator access
   // -------------------------------------------------------------------------------

   /**
    *  Returns an iterator pointing to the fist member in the object if the value type
    *  is object_type. Otherwise, throw not_object_type.
    */
   value::object_iterator value::begin_object()
   {
      object_t* p_object = boost::get<object_t>(&base_type::get());
      if( !p_object ){  boost::throw_exception( (not_object_type()) );  }
      return object_iterator(p_object->begin(),value::make_json_member());
   }

   value::const_object_iterator value::begin_object() const
   {
      const object_t* p_object = boost::get<object_t>(&base_type::get());
      if( !p_object ){  boost::throw_exception( (not_object_type()) );  }
      return const_object_iterator(p_object->begin(),value::make_json_member());
   }

   /**
    *  Returns an iterator that can be compared to determine the end of members in an object. If
    *  the value object_type is in an object then throw not_object_type.
    */
   value::object_iterator value::end_object()
   {
      object_t* p_object = boost::get<object_t>(&base_type::get());
      if( !p_object ){  boost::throw_exception( not_object_type() );  }
      return object_iterator(p_object->end(),value::make_json_member());
   }

   value::const_object_iterator value::end_object() const
   {
      const object_t* p_object = boost::get<object_t>(&base_type::get());
      if( !p_object ){  boost::throw_exception( not_object_type() );  }
      return const_object_iterator(p_object->end(),value::make_json_member());
   }

   /**
    *  Returns an iterator pointing to the fist member in the array if the value type
    *  is array_type. Otherwise, throw not_array_type.
    */
   value::array_iterator value::begin_array()
   {
      array_t* p_array = boost::get<array_t>(&base_type::get());
      if( !p_array ){  boost::throw_exception( not_array_type() );   }
      return array_iterator(p_array->begin(),value::make_json_value());
   }

   value::const_array_iterator value::begin_array() const
   {
      const array_t* p_array = boost::get<array_t>(&base_type::get());
      if( !p_array ){  boost::throw_exception( not_array_type() );   }
      return const_array_iterator(p_array->begin(),value::make_json_value());
   }

   /**
    *  Returns an iterator that can be compared to determine the end of the array. If
    *  the value type is in an array_type then throw not_array_type.
    */
   value::array_iterator value::end_array()
   {
      array_t* p_array = boost::get<array_t>(&base_type::get());
      if( !p_array ){  boost::throw_exception( not_array_type() );   }
      return array_iterator(p_array->end(),value::make_json_value());
   }

   value::const_array_iterator value::end_array() const
   {
      const array_t* p_array = boost::get<array_t>(&base_type::get());
      if( !p_array ){  boost::throw_exception( not_array_type() );   }
      return const_array_iterator(p_array->end(),value::make_json_value());
   }

   // -------------------------------------------------------------------------------
   // -------------------------------------------------------------------------------


   // -------------------------------------------------------------------------------
   //    array handling
   // -------------------------------------------------------------------------------


   value& value::add( const char* v )
   {
      return add( string_t(v) );
   }


   value& value::push_back( const char* v )
   {
      return push_back( string_t(v) );
   }


   /**
    *  Access array element stored at index value. Will throw out_of_range exception
    *  if index is not a valid index for the container.
    */
   value& value::at( size_t index )
   {
      array_t* p_array = boost::get<array_t>(&base_type::get());

      // if this isn't an array type ... throw bad type instead of the out_of_range
      if( !p_array ){  boost::throw_exception( not_array_type() );  }
            
      // container's at method will throw out_of_range if the index is out of range
      return p_array->at(index);
   }


   const value& value::at( size_t index ) const
   {
      const array_t* p_array = boost::get<array_t>(&base_type::get());

      // if this isn't an array type ... throw bad type instead of the out_of_range
      if( !p_array ){  boost::throw_exception( not_array_type() );  }
            
      // container's at method will throw out_of_range if the index is out of range
      return p_array->at(index);
   }


   /**
    *  Access array element stored at index value. If the array length is less than
    *  index, the array will be expanded to index + 1 length and additional
    *  locations will be filled with null_type.
    */
   value& value::operator[](int index)
   {
      array_t* p_array = boost::get<array_t>(&base_type::get());
      
      // if this isn't an array type ... it needs to be
      if( !p_array )
      {
         base_type::get() = array_t();
         p_array = boost::get<array_t>(&base_type::get());
      }

      // make sure that we are sized appropriately for the index being requested
      if( p_array->size() < std::size_t(index+1) )
      {
         p_array->resize(index+1,null_t());
      }
            
      return (*p_array)[index];
   }

   const value& value::operator[](int index) const
   {
      const array_t* p_array = boost::get<array_t>(&base_type::get());

      // if this isn't an array type or if the index is out of range  ... we can't fix it (const)
      if( !p_array )                              {  boost::throw_exception( not_array_type() );  }
      if( p_array->size() < std::size_t(index+1) ){  throw std::out_of_range("json::value array index");         }
            
      return (*p_array)[index];
   }

   value& value::erase(int index)
   {
      array_t* p_array = boost::get<array_t>(&base_type::get());

      // if this isn't an array type or if the index is out of range then throw
      if( !p_array )                              {  boost::throw_exception( not_array_type() );  }
      if( p_array->size() < std::size_t(index+1) ){  throw std::out_of_range("json::value array index");         }
      p_array->erase( p_array->begin() + index );
            
      return *this;
   }

   // -------------------------------------------------------------------------------
   // -------------------------------------------------------------------------------


   // -------------------------------------------------------------------------------
   //    object handling
   // -------------------------------------------------------------------------------

   value& value::set( string_t const & name, const char* v )
   {
      return set( name, string_t(v) );
   }

   bool value::has_key(const std::string & key) const
   {
      object_t::key_type key_(key);
      return boost::apply_visitor(detail::has_key_visitor(key_),*this);
   }

   value& value::operator[]( const char* v )
   {
      return (*this)[string_t(v)];
   }
      
   value& value::operator[]( const string_t& v )
   {
      object_t* p_object = boost::get<object_t>(&base_type::get());

      // if this isn't an object type ... it needs to be
      if( !p_object )
      {
         base_type::get() = object_t();
         p_object = boost::get<object_t>(&base_type::get());
      }

      return (*p_object)[v];
   }  


   const value& value::operator[]( const char* v ) const
   {
      return (*this)[string_t(v)];
   }


   const value& value::operator[]( const string_t& v ) const
   {
      const object_t* p_object = boost::get<object_t>(&base_type::get());

      if( !p_object ){  boost::throw_exception( not_object_type() );  }

      object_t::const_iterator iter = p_object->find(v);

      // if we don't have a member with the requested name we throw because it is const
      if( iter == p_object->end() ){  throw std::out_of_range("json::value object member name");  }

      return iter->second;
   }  

   value& value::erase(string_t const & v)
   {
      object_t* p_object = boost::get<object_t>(&base_type::get());

      if( !p_object ){  boost::throw_exception( (not_object_type()) );  }

      p_object->erase(v);

      return *this;
   }

   // -------------------------------------------------------------------------------
   // -------------------------------------------------------------------------------



   // ----------------------------------------------------------------
   // comparisons
   // ----------------------------------------------------------------

   bool operator==(value const& a, value const& b)
   {
      return boost::apply_visitor(detail::value_equal(), a.get_ast(), b.get_ast());
   }

   bool operator!=(value const& a, value const& b)
   {
      return !(a == b);
   }

   bool operator<(value const& a, value const& b)
   {
      return boost::apply_visitor(detail::value_compare(), a.get_ast(), b.get_ast());
   }

   // ----------------------------------------------------------------
   // ----------------------------------------------------------------


   value::operator string_t() const
   {
      return boost::get<string_t>(base_type::get());
   }


   value array() { return array_t();  }
   value object(){ return object_t(); }

}}  // namespace ciere::json

