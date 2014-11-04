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

#ifndef BOOST_INTRUSIVE_DETAIL_NODE_CLONER_DISPOSER_HPP
#define BOOST_INTRUSIVE_DETAIL_NODE_CLONER_DISPOSER_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#include <boost/intrusive/link_mode.hpp>
#include <boost/intrusive/detail/ebo_functor_holder.hpp>
#include <boost/intrusive/detail/algo_type.hpp>
#include <boost/intrusive/detail/assert.hpp>

namespace boost {
namespace intrusive {
namespace detail {

template<class F, class ValueTraits, algo_types AlgoType>
struct node_cloner
   :  private ebo_functor_holder<F>
{
   typedef ValueTraits                             value_traits;
   typedef typename value_traits::node_traits      node_traits;
   typedef typename node_traits::node_ptr          node_ptr;
   typedef ebo_functor_holder<F>                   base_t;
   typedef typename get_algo< AlgoType
                            , node_traits>::type   node_algorithms;
   static const bool safemode_or_autounlink =
      is_safe_autounlink<value_traits::link_mode>::value;
   typedef typename value_traits::value_type       value_type;
   typedef typename value_traits::pointer          pointer;
   typedef typename node_traits::node              node;
   typedef typename value_traits::const_node_ptr   const_node_ptr;
   typedef typename value_traits::reference        reference;
   typedef typename value_traits::const_reference  const_reference;

   node_cloner(F f, const ValueTraits *traits)
      :  base_t(f), traits_(traits)
   {}

   // tree-based containers use this method, which is proxy-reference friendly
   node_ptr operator()(const node_ptr & p)
   {
      const_reference v = *traits_->to_value_ptr(p);
      node_ptr n = traits_->to_node_ptr(*base_t::get()(v));
      //Cloned node must be in default mode if the linking mode requires it
      if(safemode_or_autounlink)
         BOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT(node_algorithms::unique(n));
      return n;
   }

   // hashtables use this method, which is proxy-reference unfriendly
   node_ptr operator()(const node &to_clone)
   {
      const value_type &v =
         *traits_->to_value_ptr
            (pointer_traits<const_node_ptr>::pointer_to(to_clone));
      node_ptr n = traits_->to_node_ptr(*base_t::get()(v));
      //Cloned node must be in default mode if the linking mode requires it
      if(safemode_or_autounlink)
         BOOST_INTRUSIVE_SAFE_HOOK_DEFAULT_ASSERT(node_algorithms::unique(n));
      return n;
   }

   const ValueTraits * const traits_;
};

template<class F, class ValueTraits, algo_types AlgoType>
struct node_disposer
   :  private ebo_functor_holder<F>
{
   typedef ValueTraits                          value_traits;
   typedef typename value_traits::node_traits   node_traits;
   typedef typename node_traits::node_ptr       node_ptr;
   typedef ebo_functor_holder<F>                base_t;
   typedef typename get_algo< AlgoType
                            , node_traits>::type   node_algorithms;
   static const bool safemode_or_autounlink =
      is_safe_autounlink<value_traits::link_mode>::value;

   node_disposer(F f, const ValueTraits *cont)
      :  base_t(f), traits_(cont)
   {}

   void operator()(const node_ptr & p)
   {
      if(safemode_or_autounlink)
         node_algorithms::init(p);
      base_t::get()(traits_->to_value_ptr(p));
   }
   const ValueTraits * const traits_;
};

}  //namespace detail{
}  //namespace intrusive{
}  //namespace boost{

#endif //BOOST_INTRUSIVE_DETAIL_NODE_CLONER_DISPOSER_HPP
