// -*-C++-*- boost/r_heap.cc
// <!!---------------------------------------------------------------------->
// <!! Copyright (C) 1998 Dietmar Kuehl, Claas Solutions GmbH >
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
// Title:  Implementation of a Radix heap
// Version: $Id$

// --------------------------------------------------------------------------

#if !defined(BOOST_R_HEAP_CC)
#define BOOST_R_HEAP_CC 1

#include <algorithm>

#if !defined(BOOST_R_HEAP_HPP)
#  include "boost/r_heap.hpp"
#endif

namespace boost
{

// --------------------------------------------------------------------------

template <typename T, int b> unsigned long const radix_traits<T, b>::c16;
template <typename T, int b> unsigned long const radix_traits<T, b>::c8;
template <typename T, int b> unsigned long const radix_traits<T, b>::c4;
template <typename T, int b> unsigned long const radix_traits<T, b>::c2;
template <typename T, int b> unsigned long const radix_traits<T, b>::c1;
template <typename T, int b> int const radix_traits<T, b>::bits;

// --------------------------------------------------------------------------

template <typename T, typename traits>
radix_heap<T, traits>::node::node(T const &data):
  m_prev(this),
  m_next(this),
  m_data(data)
{
}

template <typename T, typename traits>
void
radix_heap<T, traits>::node::insert(node* n)
{
  m_prev = n->m_prev;
  m_prev->m_next = this;
  m_next = n;
  m_next->m_prev = this;
}

template <typename T, typename traits>
typename radix_heap<T, traits>::node*
radix_heap<T, traits>::node::remove()
{
  node* rc = m_prev->m_next = m_next;
  m_next->m_prev = m_prev;

  if (m_prev == this)
    return 0;
  m_prev = m_next = this;
  return rc;
}

// --------------------------------------------------------------------------

template <typename T, typename traits>
radix_heap<T, traits>::iterator::iterator(node* const* buckets):
  m_buckets(buckets),
  m_node(0),
  m_idx(0)
{
  while (m_buckets[m_idx] == 0)
    ++m_idx;
  m_node = m_buckets[m_idx];
}

template <typename T, typename traits>
typename radix_heap<T, traits>::iterator&
radix_heap<T, traits>::iterator::operator++()
{
  m_node = m_node->m_next;
  if (m_node == m_buckets[m_idx])
    {
      for (++m_idx; m_idx < 33; ++m_idx)
    if (m_buckets[m_idx] != 0)
      {
        m_node = m_buckets[m_idx];
        break;
      }
      m_node = 0;
    }

  return *this;
}

// --------------------------------------------------------------------------

template <typename T, typename traits>
radix_heap<T, traits>::radix_heap():
  m_offset(0),
  m_cut(1ul << m_offset),
  m_size(0)
{
  std::fill(m_buckets, m_buckets + 33, static_cast<node*>(0));
}

template <typename T, typename traits>
radix_heap<T, traits>::~radix_heap()
{
  if (m_size == 0)
    return;

  for (int i = 0; i < 33; ++i)
    if (m_buckets[i] != 0)
      {
    node* n = m_buckets[i];
    n->m_prev->m_next = 0;
    for (node* n = m_buckets[i]; n != 0; )
    {
      node* del = n;
      n = n->m_next;
      delete del;
    }
      }
}

// --------------------------------------------------------------------------

template <typename T, typename traits>
typename radix_heap<T, traits>::pointer
radix_heap<T, traits>::push(T const& val)
{
  ++m_size;
  unsigned long offset = (traits::value(val) < m_cut || m_cut == 0ul)? m_offset: 0;
  return pointer(insert(new node(val), offset));
}

template <typename T, typename traits>
typename radix_heap<T, traits>::node*
radix_heap<T, traits>::insert(node* n, unsigned long offset) const
{
  unsigned long idx = traits::radix(n->m_data, offset);

  if (m_buckets[idx] == 0)
    m_buckets[idx] = n;
  else
    n->insert(m_buckets[idx]);

  return n;
}

// --------------------------------------------------------------------------

template <typename T, typename traits>
T const&
radix_heap<T, traits>::top() const
{
  if (m_buckets[0] == 0)
    reorganize();
  return m_buckets[0]->m_data;
}

// --------------------------------------------------------------------------

template <typename T, typename traits>
void
radix_heap<T, traits>::reorganize() const
{
  node** bucket = m_buckets + 1;
  while (*bucket == 0)
    ++bucket;

  node* start = *bucket;
  *bucket = static_cast<node*>(0);
  m_cut = 1ul << bucket - m_buckets;

  unsigned long min = traits::value(start->m_data);
  for (node* tmp = start->m_next; tmp != start; tmp = tmp->m_next)
    if (traits::value(tmp->m_data) < min)
      min = traits::value(tmp->m_data);

  m_offset = min;
  do
    {
      node* tmp = start;
      start = start->remove();
      insert(tmp, m_offset);
    }
  while (start != 0);
}

// --------------------------------------------------------------------------

template <typename T, typename traits>
void
radix_heap<T, traits>::pop()
{
  if (m_buckets[0] == 0)
    reorganize();
  m_buckets[0] = m_buckets[0]->remove();
  if (--m_size == 0)
    {
      m_offset = 0;
      m_cut = 1ul << m_offset;
    }
}

// --------------------------------------------------------------------------

} // namespace boost

#endif /* BOOST_R_HEAP_CC */
