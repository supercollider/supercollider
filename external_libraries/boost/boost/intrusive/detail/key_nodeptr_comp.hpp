/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2014-2014
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_DETAIL_KEY_NODEPTR_COMP_HPP
#define BOOST_INTRUSIVE_DETAIL_KEY_NODEPTR_COMP_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/detail/ebo_functor_holder.hpp>

namespace boost {
namespace intrusive {
namespace detail {

template<class KeyValueCompare, class ValueTraits>
struct key_nodeptr_comp
   :  private ebo_functor_holder<KeyValueCompare>
{
   typedef ValueTraits                             value_traits;
   typedef typename value_traits::value_type       value_type;
   typedef typename value_traits::node_ptr         node_ptr;
   typedef typename value_traits::const_node_ptr   const_node_ptr;
   typedef ebo_functor_holder<KeyValueCompare>     base_t;

   key_nodeptr_comp(KeyValueCompare kcomp, const ValueTraits *traits)
      :  base_t(kcomp), traits_(traits)
   {}

   template<class T>
   struct is_node_ptr
   {
      static const bool value = is_same<T, const_node_ptr>::value || is_same<T, node_ptr>::value;
   };

   template<class T>
   const value_type & key_forward
      (const T &node, typename enable_if_c<is_node_ptr<T>::value>::type * = 0) const
   {  return *traits_->to_value_ptr(node);  }

   template<class T>
   const T & key_forward(const T &key, typename enable_if_c<!is_node_ptr<T>::value>::type* = 0) const
   {  return key;  }


   template<class KeyType, class KeyType2>
   bool operator()(const KeyType &key1, const KeyType2 &key2) const
   {  return base_t::get()(this->key_forward(key1), this->key_forward(key2));  }

   template<class KeyType>
   bool operator()(const KeyType &key1) const
   {  return base_t::get()(this->key_forward(key1));  }

   const ValueTraits *const traits_;
};

}  //namespace detail{
}  //namespace intrusive{
}  //namespace boost{

#endif //BOOST_INTRUSIVE_DETAIL_KEY_NODEPTR_COMP_HPP
