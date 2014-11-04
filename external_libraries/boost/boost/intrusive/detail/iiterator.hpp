/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2006-2014
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_DETAIL_IITERATOR_HPP
#define BOOST_INTRUSIVE_DETAIL_IITERATOR_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#include <boost/intrusive/pointer_traits.hpp>
#include <boost/intrusive/detail/mpl.hpp>
#include <boost/intrusive/detail/is_stateful_value_traits.hpp>
#include <boost/intrusive/detail/std_fwd.hpp>

#include <cstddef>


namespace boost {
namespace intrusive {

template<class Category, class T, class Distance, class Pointer, class Reference>
struct iterator
{
   typedef Category  iterator_category;
   typedef T         value_type;
   typedef Distance  difference_type;
   typedef Pointer   pointer;
   typedef Reference reference;
};

template<class Iterator>
struct iterator_traits
{
   typedef typename Iterator::difference_type   difference_type;
   typedef typename Iterator::value_type        value_type;
   typedef typename Iterator::pointer           pointer;
   typedef typename Iterator::reference         reference;
   typedef typename Iterator::iterator_category iterator_category;
};

template<class T>
struct iterator_traits<T*>
{
   typedef std::ptrdiff_t                    difference_type;
   typedef T                                 value_type;
   typedef T*                                pointer;
   typedef T&                                reference;
   typedef std::random_access_iterator_tag   iterator_category;
};

template<class T>
struct iterator_traits<const T*>
{
   typedef std::ptrdiff_t                    difference_type;
   typedef T                                 value_type;
   typedef const T*                          pointer;
   typedef const T&                          reference;
   typedef std::random_access_iterator_tag   iterator_category;
};

template<class ValueTraits>
struct value_traits_pointers
{
   typedef BOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT
      (boost::intrusive::detail::
      , ValueTraits, value_traits_ptr
      , typename pointer_traits<typename ValueTraits::node_traits::node_ptr>::template
         rebind_pointer<ValueTraits>::type)   value_traits_ptr;

   typedef typename pointer_traits<value_traits_ptr>::template
      rebind_pointer<ValueTraits const>::type const_value_traits_ptr;
};

template<class ValueTraits, bool IsConst, class Category>
struct iiterator
{
   typedef ValueTraits                                         value_traits;
   typedef typename value_traits::node_traits                  node_traits;
   typedef typename node_traits::node                          node;
   typedef typename node_traits::node_ptr                      node_ptr;
   typedef ::boost::intrusive::pointer_traits<node_ptr>        nodepointer_traits_t;
   typedef typename nodepointer_traits_t::template
      rebind_pointer<void>::type                               void_pointer;
   typedef typename ValueTraits::value_type                    value_type;
   typedef typename ValueTraits::pointer                       nonconst_pointer;
   typedef typename ValueTraits::const_pointer                 yesconst_pointer;
   typedef typename ::boost::intrusive::pointer_traits
      <nonconst_pointer>::reference                            nonconst_reference;
   typedef typename ::boost::intrusive::pointer_traits
      <yesconst_pointer>::reference                            yesconst_reference;
   typedef typename nodepointer_traits_t::difference_type      difference_type;
   typedef typename detail::if_c
      <IsConst, yesconst_pointer, nonconst_pointer>::type      pointer;
   typedef typename detail::if_c
      <IsConst, yesconst_reference, nonconst_reference>::type  reference;
   typedef iterator
         < Category
         , value_type
         , difference_type
         , pointer
         , reference
         > iterator_traits;
   typedef typename value_traits_pointers
      <ValueTraits>::value_traits_ptr                          value_traits_ptr;
   typedef typename value_traits_pointers
      <ValueTraits>::const_value_traits_ptr                    const_value_traits_ptr;
   static const bool stateful_value_traits =
      detail::is_stateful_value_traits<value_traits>::value;
};

template<class NodePtr, class StoredPointer, bool StatefulValueTraits = true>
struct iiterator_members
{

   iiterator_members()
      : nodeptr_()//Value initialization to achieve "null iterators" (N3644)
   {}

   iiterator_members(const NodePtr &n_ptr, const StoredPointer &data)
      :  nodeptr_(n_ptr), ptr_(data)
   {}

   StoredPointer get_ptr() const
   {  return ptr_;  }

   NodePtr nodeptr_;
   StoredPointer ptr_;
};

template<class NodePtr, class StoredPointer>
struct iiterator_members<NodePtr, StoredPointer, false>
{
   iiterator_members()
      : nodeptr_()//Value initialization to achieve "null iterators" (N3644)
   {}

   iiterator_members(const NodePtr &n_ptr, const StoredPointer &)
      : nodeptr_(n_ptr)
   {}

   StoredPointer get_ptr() const
   {  return StoredPointer();  }

   NodePtr nodeptr_;
};

namespace detail {

template<class InputIt, class Distance> inline
void advance_impl(InputIt& it, Distance n, const std::input_iterator_tag&)
{
   while(n--)
	   ++it;
}

template<class InputIt, class Distance> inline
void advance_impl(InputIt& it, Distance n, std::forward_iterator_tag &)
{
   while(n--)
	   ++it;
}

template<class InputIt, class Distance> inline
void advance_impl(InputIt& it, Distance n, std::bidirectional_iterator_tag &)
{
   for (; 0 < n; --n)
	   ++it;
   for (; n < 0; ++n)
	   --it;
}

template<class InputIt, class Distance>
inline void advance_impl(InputIt& it, Distance n, const std::random_access_iterator_tag &)
{
   it += n;
}

} //namespace detail

template<class InputIt, class Distance>
inline void iterator_advance(InputIt& it, Distance n)
{	// increment iterator by offset, arbitrary iterators
   boost::intrusive::detail::advance_impl(it, n, boost::intrusive::iterator_traits<InputIt>::iterator_category());
}

namespace detail{

template<class InputIt, class Distance, class Category>
inline void distance_impl(InputIt first, InputIt last, Distance& off, const Category &)
{
   while(first != last){
	   ++off;
      ++first;
   }
}

template<class InputIt, class Distance> inline
void distance_impl(InputIt first, InputIt last, Distance& off, const std::random_access_iterator_tag&)
{
   off += last - first;
}

}  //namespace detail

template<class InputIt> inline
typename iterator_traits<InputIt>::difference_type iterator_distance(InputIt first, InputIt last)
{
   typename iterator_traits<InputIt>::difference_type off = 0;
   boost::intrusive::detail::distance_impl(first, last, off, boost::intrusive::iterator_traits<InputIt>::iterator_category());
   return off;
}


} //namespace intrusive
} //namespace boost

#endif //BOOST_INTRUSIVE_DETAIL_IITERATOR_HPP
