#ifndef BOOST_ATOMIC_DETAIL_TYPE_CLASSIFIER_HPP
#define BOOST_ATOMIC_DETAIL_TYPE_CLASSIFIER_HPP

//  Copyright (c) 2011 Helge Bahmann
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

namespace boost {
namespace atomics {
namespace detail {

template<typename T>
struct type_classifier {
    typedef void test;
};

template<>
struct type_classifier<char> {typedef int test;};
template<>
struct type_classifier<unsigned char> {typedef int test;};
template<>
struct type_classifier<signed char> {typedef int test;};
template<>
struct type_classifier<unsigned short> {typedef int test;};
template<>
struct type_classifier<signed short> {typedef int test;};
template<>
struct type_classifier<unsigned int> {typedef int test;};
template<>
struct type_classifier<signed int> {typedef int test;};
template<>
struct type_classifier<unsigned long> {typedef int test;};
template<>
struct type_classifier<long> {typedef int test;};
#ifdef BOOST_HAS_LONG_LONG
template<> struct type_classifier<unsigned long long>
{typedef int test;};
template<> struct type_classifier<signed long long>
{typedef int test;};
#endif

template<typename T>
struct type_classifier<T *> {typedef void * test;};

template<typename T>
struct sign_trait {
    typedef void test;
};

template<>
struct sign_trait<char> {typedef int test;};
template<>
struct sign_trait<unsigned char> {typedef unsigned int test;};
template<>
struct sign_trait<signed char> {typedef int test;};
template<>
struct sign_trait<unsigned short> {typedef unsigned int test;};
template<>
struct sign_trait<signed short> {typedef int test;};
template<>
struct sign_trait<unsigned int> {typedef unsigned int test;};
template<>
struct sign_trait<signed int> {typedef int test;};
template<>
struct sign_trait<unsigned long> {typedef unsigned int test;};
template<>
struct sign_trait<long> {typedef int test;};
#ifdef BOOST_HAS_LONG_LONG
template<> struct sign_trait<unsigned long long>
{typedef unsigned int test;};
template<> struct sign_trait<signed long long>
{typedef int test;};
#endif



}}}

#endif
