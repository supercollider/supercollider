//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONTAINERS_DETAIL_UTILITIES_HPP
#define BOOST_CONTAINERS_DETAIL_UTILITIES_HPP

#include <boost/container/detail/config_begin.hpp>
#include <cstdio>
#include <algorithm>

namespace boost {
namespace container {
namespace containers_detail {

template <class SizeType>
SizeType
   get_next_capacity(const SizeType max_size
                    ,const SizeType capacity
                    ,const SizeType n)
{
//   if (n > max_size - capacity)
//      throw std::length_error("get_next_capacity");

   const SizeType m3 = max_size/3;

   if (capacity < m3)
      return capacity + max_value(3*(capacity+1)/5, n);

   if (capacity < m3*2)
      return capacity + max_value((capacity+1)/2, n);

   return max_size;
}

template<class T>
const T &max_value(const T &a, const T &b)
{  return a > b ? a : b;   }

template<class T>
const T &min_value(const T &a, const T &b)
{  return a < b ? a : b;   }

template<class SmartPtr>
struct smart_ptr_type
{
   typedef typename SmartPtr::value_type value_type;
   typedef value_type *pointer;
   static pointer get (const SmartPtr &smartptr)
   {  return smartptr.get();}
};

template<class T>
struct smart_ptr_type<T*>
{
   typedef T value_type;
   typedef value_type *pointer;
   static pointer get (pointer ptr)
   {  return ptr;}
};

//!Overload for smart pointers to avoid ADL problems with get_pointer
template<class Ptr>
inline typename smart_ptr_type<Ptr>::pointer
get_pointer(const Ptr &ptr)
{  return smart_ptr_type<Ptr>::get(ptr);   }

//!To avoid ADL problems with swap
template <class T>
inline void do_swap(T& x, T& y)
{
   using std::swap;
   swap(x, y);
}

template <std::size_t OrigSize, std::size_t RoundTo>
struct ct_rounded_size
{
   enum { value = ((OrigSize-1)/RoundTo+1)*RoundTo };
};

}  //namespace containers_detail {
}  //namespace container {
}  //namespace boost {


#include <boost/container/detail/config_end.hpp>

#endif   //#ifndef BOOST_CONTAINERS_DETAIL_UTILITIES_HPP
