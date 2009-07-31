// -*-C++-*- boost/p_queue.hpp
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
// Title:  Declaration for an extended version of priority_queue
// Version: $Id$ 

// -------------------------------------------------------------------------- 

#if !defined(BOOST_P_QUEUE_HPP)
#define BOOST_P_QUEUE_HPP 1

#include <functional>
#include <vector>

namespace boost
{

template <typename T,
          typename Cont = std::vector<T>,
          typename Comp = std::less<typename Cont::value_type> >
class priority_queue
{
public:
  typedef typename Cont::value_type      value_type;
  typedef typename Cont::size_type       size_type;
  typedef          Cont                  container_type;
  typedef typename Cont::reference       reference;
  typedef typename Cont::const_reference const_reference;

  typedef typename Cont::iterator        iterator;
  typedef typename Cont::const_iterator  const_iterator;

  explicit priority_queue(Comp const& comp = Comp()): m_compare(comp) {}

  bool            empty() const { return m_container.empty(); }
  size_type       size() const { return m_container.size(); }
  const_reference top() const { return m_container[0]; }

  void push(const_reference val);
  void pop();
  template <typename K> void change_top(K const& val);

  const_iterator begin() const { return m_container.begin(); }
  const_iterator end() const   { return m_container.end(); }

private:
  void siftup(size_type idx, const_reference value);

  Cont m_container;
  Comp m_compare;
};

// --------------------------------------------------------------------------

}

#if !defined(BOOST_P_QUEUE_CC)
#  include "boost/p_queue.cc"
#endif

#endif /* BOOST_P_QUEUE_HPP */
