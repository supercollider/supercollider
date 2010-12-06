// -*-C++-*- boost/p_queue.cc
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
// Title:  Implementation of the non-trivial members of the priority queue
// Version: $Id$ 

// -------------------------------------------------------------------------- 

#if !defined(BOOST_P_QUEUE_CC)
#define BOOST_P_QUEUE_CC 1

#if !defined(BOOST_P_QUEUE_HPP)
#  include "boost/p_queue.hpp"
#endif

namespace boost
{

// --------------------------------------------------------------------------

template <typename T, typename Cont, typename Comp>
inline void
  priority_queue<T, Cont, Comp>::siftup(size_type idx, const_reference value)
  {
    if (idx == 0)
      return;

    iterator base = m_container.begin();
    for (size_type pidx = (idx - 1) / 2;
         idx != 0 && m_compare(*(base + pidx), value);
         idx = pidx, pidx = (pidx - 1) / 2)
      *(base + idx) = *(base + pidx);
    *(base + idx) = value;
  }

// -----------------------------------------------------------------------------

template <typename T, typename Cont, typename Comp>
inline void
  priority_queue<T, Cont, Comp>::push(const_reference val)
  {
    m_container.push_back(val);
    siftup(m_container.size() - 1, val);
  }

// -----------------------------------------------------------------------------

template <typename T, typename Cont, typename Comp>
inline void
  priority_queue<T, Cont, Comp>::pop()
  {
    size_type len  = m_container.size();
    iterator  base = m_container.begin();
    size_type hidx = 0;
    size_type cidx = 2;

    for ( ; cidx < len; hidx = cidx, cidx = cidx * 2 + 2)
      {
        if (m_compare(*(base + cidx), *(base + (cidx - 1))))
          --cidx;
        *(base + hidx) = *(base + cidx);
      }

    *(base + hidx) = *(base + (len - 1));
    if (cidx != len)
      siftup(hidx, *(base + (len - 1)));

    m_container.pop_back();
  }

// --------------------------------------------------------------------------

template <typename T, typename Cont, typename Comp>
template <typename K>
inline void
  priority_queue<T, Cont, Comp>::change_top(K const& val)
  {
    T tmp(m_container[0]);
    tmp = val;

    size_type len  = m_container.size();
    iterator  base = m_container.begin();
    size_type hidx = 0;
    size_type cidx = 2;

    for ( ; cidx < len; hidx = cidx, cidx = cidx * 2 + 2)
      {
        if (m_compare(*(base + cidx), *(base + (cidx - 1))))
          --cidx;
	if (m_compare(*(base + cidx), tmp))
	  break;
        *(base + hidx) = *(base + cidx);
      }

    *(base + hidx) = tmp;
  }

// --------------------------------------------------------------------------

}

#endif /* BOOST/P_QUEUE_CC */
