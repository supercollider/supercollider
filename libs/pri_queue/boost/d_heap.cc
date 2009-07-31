// -*-C++-*- boost/d_heap.cc
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
// Title:  Implementation of a d-heap
// Version: $Id$

// See "Network Flows", R.K.Ahuja & T.L.Magnanti & J.B.Orlin, Prentice Hall,
// for details on the logic used.

// --------------------------------------------------------------------------

#if !defined(BOOST_D_HEAP_CC)
#define BOOST_D_HEAP_CC 1

#if !defined(BOOST_D_HEAP_HPP)
#  include "boost/d_heap.hpp"
#endif

namespace boost
{

// --------------------------------------------------------------------------

template <typename T, typename Comp, int d>
inline void
  d_heap<T, Comp, d>::siftup(size_type idx, pointer ptr)
  {
    if (idx == 0)
      return;

    typename std::vector<node_iterator>::iterator base = d_heap_base<T>::m_heap.begin();
    for (size_type pidx = (idx - 1) / d;
         idx != 0 && m_compare((*(base + pidx))->m_value, ptr->m_value);
         idx = pidx, pidx = (pidx - 1) / d)
      {
    *(base + idx) = *(base + pidx);
    (*(base + idx))->m_index = idx;
      }
    *(base + idx) = ptr;
    (*(base + idx))->m_index = idx;
  }

// -----------------------------------------------------------------------------

template <typename T, typename Comp, int d>
inline typename d_heap<T, Comp, d>::pointer d_heap<T, Comp, d>::push(const_reference value)
{
  d_heap_base<T>::m_nodes.push_back(node(d_heap_base<T>::m_heap.size(), value));
  d_heap_base<T>::m_heap.push_back(--d_heap_base<T>::m_nodes.end());
  siftup(d_heap_base<T>::m_heap.size() - 1, --d_heap_base<T>::m_nodes.end());

  return --d_heap_base<T>::m_nodes.end();
}

// -----------------------------------------------------------------------------

template <typename T, typename Comp, int d>
inline void
  d_heap<T, Comp, d>::pop()
  {
      size_type                                     len  = d_heap_base<T>::m_heap.size();
    typename std::vector<node_iterator>::iterator base = d_heap_base<T>::m_heap.begin();
    node_iterator                                 rmnode = *base;
    size_type                                     hidx = 0;
    size_type                                     cidx = d;

    while (cidx < len)
      {
        size_type i = 1;
    size_type midx = cidx;

    for (; i < d; ++i)
      if (m_compare((*(base + midx))->m_value, (*(base + (cidx - i)))->m_value))
        midx = cidx - i;

        *(base + hidx) = *(base + midx);
        (*(base + hidx))->m_index = hidx;

    hidx = midx;
    cidx = midx * d + d;
      }

    if ((cidx + 1) - d < len)
      {
    cidx = (cidx + 1) - d;
    size_type midx = cidx;
    while (++cidx < len)
      if (m_compare((*(base + midx))->m_value, (*(base + cidx))->m_value))
        midx = cidx;

        *(base + hidx) = *(base + midx);
        (*(base + hidx))->m_index = hidx;

    hidx = midx;
      }

    *(base + hidx) = *(base + (len - 1));
    (*(base + hidx))->m_index = hidx;

    siftup(hidx, *(base + (len - 1)));

    d_heap_base<T>::m_nodes.erase(rmnode);
    d_heap_base<T>::m_heap.pop_back();
  }

template <typename T, typename Comp, int d>
inline void
  d_heap<T, Comp, d>::remove(pointer p)
  {
    size_type                                     len  = d_heap_base<T>::m_heap.size();
    typename std::vector<node_iterator>::iterator base = d_heap_base<T>::m_heap.begin();
    node_iterator                                 rmnode = *base;
    size_type                                     hidx = p->m_index;
    size_type                                     cidx = hidx * d + d;

    while (cidx < len)
      {
        size_type i = 1;
    size_type midx = cidx;

    for (; i < d; ++i)
      if (m_compare((*(base + midx))->m_value, (*(base + (cidx - i)))->m_value))
        midx = cidx - i;

        *(base + hidx) = *(base + midx);
        (*(base + hidx))->m_index = hidx;

    hidx = midx;
    cidx = midx * d + d;
      }

    *(base + hidx) = *(base + (len - 1));
    (*(base + hidx))->m_index = hidx;

    siftup(hidx, *(base + (len - 1)));

    d_heap_base<T>::m_nodes.erase(rmnode);
    d_heap_base<T>::m_heap.pop_back();
  }


// --------------------------------------------------------------------------

template <typename T, typename Comp, int d>
template <typename K>
inline void
  d_heap<T, Comp, d>::increase(pointer p, K const& value)
  {
    p->m_value = value;
    siftup(p->m_index, p);
  }

template <typename T, typename Comp, int d>
template <typename K>
inline void
  d_heap<T, Comp, d>::decrease(pointer p, K const& value)
  {
    size_type                                     len  = d_heap_base<T>::m_heap.size();
    typename std::vector<node_iterator>::iterator base = d_heap_base<T>::m_heap.begin();
    size_type                                     hidx = p->m_index;
    size_type                                     cidx = hidx * d + d;

    p->m_value = value;

    while (cidx < len)
      {
    pointer min = p;
    for (size_type i = 0; i < d; ++i)
      if (m_compare(min->m_value, (*(base + (cidx - i)))->m_value))
        min = *(base + (cidx - i));

    if (min == p)
      break;

    cidx = min->m_index;

        *(base + hidx) = min;
        min->m_index = hidx;

    hidx = cidx;
    cidx = cidx * d + d;
      }

    *(base + hidx) = p;
    p->m_index = hidx;
  }

template <typename T, typename Comp, int d>
template <typename K>
inline void
  d_heap<T, Comp, d>::change(pointer p, K const& value)
  {
    T comp(p->m_value);
    comp = value;

    if (m_compare(p->m_value, comp))
      increase(p, value);
    else
      decrease(p, value);
  }

template <typename T, typename Comp, int d>
template <typename K>
inline void
  d_heap<T, Comp, d>::change_top(K const& value)
  {
      change(d_heap_base<T>::m_heap[0], value);
  }

// --------------------------------------------------------------------------

} // namespace boost

#endif /* BOOST_D_HEAP_CC */
