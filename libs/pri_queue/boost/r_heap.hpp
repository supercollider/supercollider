// -*-C++-*- boost/r_heap.hpp
// <!!---------------------------------------------------------------------->
// <!! Copyright (C) 1999 Dietmar Kuehl, Claas Solutions GmbH >
// <!!>
// <!! Permission to use, copy, modify, distribute and sell this >
// <!! software for any purpose is hereby granted without fee, provided >
// <!! that the above copyright notice appears in all copies and that >
// <!! both that copyright notice and this permission notice appear in >
// <!! supporting documentation. Dietmar Kuehl and Claas Solutions make no >
// <!! representations about the suitability of this software for any >
// <!! purpose. It is provided "as is" without express or implied warranty. >
// <!!---------------------------------------------------------------------->

// Author: Dietmar Kuehl dietmar.kuehl@claas-solutions.de
// Title:  Declarations for a Radix heap implementation
// Version: $Id$

// --------------------------------------------------------------------------

#if !defined(BOOST_R_HEAP_HPP)
#define BOOST_R_HEAP_HPP 1

namespace boost
{

// --------------------------------------------------------------------------
// An implementation of radix heaps supposed to be better for special
// applications but also implying some restrictions:
// - The keys are supposed to be positive integer numbers
// - The lower bound for key of inserted nodes or decreased keys
//   is the key of the most recently accessed node (using top() or pop())
// The current implementation uses types convertible to unsigned long
// as key values. This might be relaxed in the future.

// NOTE: Currently, this implementation is limited to 32 bit longs!
//       To change this, it is necessary to adjust the method
//       'intern_radix()' below (it should be obvious how to do so :-)

template <typename T, int b = 32>
class radix_traits
{
private:
  static unsigned long const c16 = 0xffff0000ul;
  static unsigned long const c8 =  0xff00ff00ul;
  static unsigned long const c4 =  0xf0f0f0f0ul;
  static unsigned long const c2 =  0xccccccccul;
  static unsigned long const c1 =  0xaaaaaaaaul;

  static int intern_radix(unsigned long val)
  {
    if (val == 0)
      return 0;

    int rc = 1;

    if (bits > 16 && (val & c16) != 0) { val &= c16; rc += 16; }
    if (bits > 8  && (val & c8)  != 0) { val &= c8;  rc += 8; }
    if (bits > 4  && (val & c4)  != 0) { val &= c4;  rc += 4; }
    if (bits > 2  && (val & c2)  != 0) { val &= c2;  rc += 2; }
    if (bits > 1  && (val & c1)  != 0) { val &= c1;  rc += 1; }

    return rc;
  }

public:
  static int const bits = b;

  static unsigned long value(T val)
  {
    return 0xffffffff - static_cast<unsigned long>(val);
  }
  static int radix(T val, unsigned long offset)
  {
    return intern_radix(value(val) - offset);
  }
};

template <typename T, typename traits = radix_traits<T> >
class radix_heap
{
  struct node
  {
    node(T const& data);

    void  insert(node* n);
    node* remove();

    T &operator* () { return m_data; }

    node* m_prev;
    node* m_next;
    T     m_data;
  };

public:
  class iterator
  {
    friend class radix_heap;
    explicit iterator(node* const* buckets);

  public:
    iterator(): m_buckets(0), m_node(0), m_idx(0) {}

    T const& operator* () const { return m_node->m_data; }
    T const* operator->() const { return &m_node->m_data; }

    iterator& operator++();
    iterator  operator++(int)
      {
    iterator rc(*this);
    operator++();
    return rc;
      }

    bool operator== (iterator const& it) const { return m_node == it.m_node; }
    bool operator!= (iterator const& it) const { return m_node != it.m_node; }

  private:
    node* const* m_buckets;
    node*        m_node;
    int          m_idx;
  };

  typedef T        value_type;
  typedef T&       reference;
  typedef T const& const_reference;
  typedef iterator const_iterator;
  typedef int      size_type;
  typedef node*    pointer;

  radix_heap();
  ~radix_heap();

  void      remove(pointer);
  void      change(pointer, T const&);
  void      change_top(T const&);
  void      decrease(pointer, T const&);
  void      increase(pointer, T const&);

  bool      empty() const { return m_size == 0; }
  size_type size() const { return m_size; }
  pointer  push(T const&);
  T const&  top() const;
  void      pop();

  iterator begin() const { return empty()? iterator(): iterator(m_buckets); }
  iterator end() const { return iterator(); }

private:
  node* insert(node*, unsigned long) const; // called from reorganize()
  void  reorganize() const; // called by top() and pop()

  mutable node*         m_buckets[33];
  mutable unsigned long m_offset;
  mutable unsigned long m_cut;
  size_type             m_size;

  radix_heap(radix_heap const&);      // deliberately not implemented
  void operator= (radix_heap const&); // deliberately not implemented
};

// --------------------------------------------------------------------------

} // namespace boost

#if !defined(BOOST_R_HEAP_CC)
#  include "boost/r_heap.cc"
#endif

#endif /* BOOST_R_HEAP_HPP */
