//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONTAINERS_CONTAINERS_FWD_HPP
#define BOOST_CONTAINERS_CONTAINERS_FWD_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

//////////////////////////////////////////////////////////////////////////////
//                        Standard predeclarations
//////////////////////////////////////////////////////////////////////////////

/// @cond

namespace boost{ 
namespace intrusive{ 
   //Create namespace to avoid compilation errors
}}

namespace boost{ namespace container{ namespace containers_detail{

namespace bi = boost::intrusive;

}}}

namespace std {

template <class T>
class allocator;

template <class T>
struct less;

template <class T1, class T2>
struct pair;

template <class CharType> 
struct char_traits;

}  //namespace std {

/// @endcond

//////////////////////////////////////////////////////////////////////////////
//                             Containers
//////////////////////////////////////////////////////////////////////////////

#ifdef BOOST_CONTAINER_DOXYGEN_INVOKED
namespace boost {
namespace container {
#else
namespace boost {
namespace container {
#endif

//vector class
template <class T
         ,class A = std::allocator<T> >
class vector;

//vector class
template <class T
,class A = std::allocator<T> >
class deque;

//list class
template <class T
         ,class A = std::allocator<T> >
class list;

//slist class
template <class T
         ,class Alloc = std::allocator<T> >
class slist;

//set class
template <class T
         ,class Pred  = std::less<T>
         ,class Alloc = std::allocator<T> >
class set;

//multiset class
template <class T
         ,class Pred  = std::less<T>
         ,class Alloc = std::allocator<T> >
class multiset;

//map class
template <class Key
         ,class T
         ,class Pred  = std::less<Key>
         ,class Alloc = std::allocator<std::pair<const Key, T> > >
class map;

//multimap class
template <class Key
         ,class T
         ,class Pred  = std::less<Key>
         ,class Alloc = std::allocator<std::pair<const Key, T> > >
class multimap;

//flat_set class
template <class T
         ,class Pred  = std::less<T>
         ,class Alloc = std::allocator<T> >
class flat_set;

//flat_multiset class
template <class T
         ,class Pred  = std::less<T>
         ,class Alloc = std::allocator<T> >
class flat_multiset;

//flat_map class
template <class Key
         ,class T
         ,class Pred  = std::less<Key>
         ,class Alloc = std::allocator<std::pair<Key, T> > >
class flat_map;

//flat_multimap class
template <class Key
         ,class T
         ,class Pred  = std::less<Key>
         ,class Alloc = std::allocator<std::pair<Key, T> > >
class flat_multimap;

//basic_string class
template <class CharT
         ,class Traits = std::char_traits<CharT>
         ,class Alloc  = std::allocator<CharT> > 
class basic_string;

//string class
typedef basic_string
   <char
   ,std::char_traits<char>
   ,std::allocator<char> >
string;

}}  //namespace boost { namespace container {

#endif //#ifndef BOOST_CONTAINERS_CONTAINERS_FWD_HPP

