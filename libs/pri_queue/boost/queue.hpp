// -*-C++-*- boost/queue.h
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
// Title:  A queue adaptor with extended functionality compared to the standard one
// Version: $Id$ 

// -------------------------------------------------------------------------- 

#if !defined(BOOST_QUEUE_H)
#define BOOST_QUEUE_H 1

#include <deque>

namespace boost
{

template <typename T, typename Cont = std::deque<T> >
class queue;

template <typename T, typename Cont>
bool operator== (queue<T, Cont> const&, queue<T, Cont> const&);
template <typename T, typename Cont>
bool operator!= (queue<T, Cont> const&, queue<T, Cont> const&);
template <typename T, typename Cont>
bool operator<= (queue<T, Cont> const&, queue<T, Cont> const&);
template <typename T, typename Cont>
bool operator>= (queue<T, Cont> const&, queue<T, Cont> const&);
template <typename T, typename Cont>
bool operator<  (queue<T, Cont> const&, queue<T, Cont> const&);
template <typename T, typename Cont>
bool operator>  (queue<T, Cont> const&, queue<T, Cont> const&);

// --------------------------------------------------------------------------

template <typename T, typename Cont>
class queue
{
  friend bool operator== <T, Cont>(queue<T, Cont> const&, queue<T, Cont> const&);
  friend bool operator!= <T, Cont>(queue<T, Cont> const&, queue<T, Cont> const&);
  friend bool operator<= <T, Cont>(queue<T, Cont> const&, queue<T, Cont> const&);
  friend bool operator>= <T, Cont>(queue<T, Cont> const&, queue<T, Cont> const&);
  friend bool operator<  <T, Cont>(queue<T, Cont> const&, queue<T, Cont> const&);
  friend bool operator>  <T, Cont>(queue<T, Cont> const&, queue<T, Cont> const&);

public:
  typedef typename Cont::value_type      value_type;
  typedef typename Cont::reference       reference;
  typedef typename Cont::const_reference const_reference;
  typedef typename Cont::size_type       size_type;
  typedef Cont                           container_type;
  typedef typename Cont::iterator        iterator;
  typedef typename Cont::const_iterator  const_iterator;

  explicit queue(Cont const& cont = Cont()): m_container(cont) {}

  bool              empty() const               { return m_container.empty(); }
  size_type         size() const                { return m_container.size(); }
  void              push(value_type const& val) { m_container.push_back(val); }
  void              pop()                       { m_container.pop_front(); }
  value_type const& top() const                 { return m_container.front(); }
  value_type&       top()                       { return m_container.front(); }
  value_type const& front() const               { return m_container.front(); }
  value_type&       front()                     { return m_container.front(); }
  value_type const& back() const                { return m_container.back(); }
  value_type&       back()                      { return m_container.back(); }

  iterator       begin()       { return m_container.begin(); }
  iterator       end()         { return m_container.end(); }
  const_iterator begin() const { return m_container.begin(); }
  const_iterator end() const   { return m_container.end(); }

private:
  Cont m_container;
};

template <typename T, typename Cont>
bool
operator== (queue<T, Cont> const& q1, queue<T, Cont> const&q2)
{
  return q1 == q2;
}

template <typename T, typename Cont>
bool
operator!= (queue<T, Cont> const& q1, queue<T, Cont> const& q2)
{
  return q1 != q2;
}

template <typename T, typename Cont>
bool
operator<= (queue<T, Cont> const& q1, queue<T, Cont> const& q2)
{
  return q1 <= q2;
}

template <typename T, typename Cont>
bool
operator>= (queue<T, Cont> const& q1, queue<T, Cont> const& q2)
{
  return q1 >= q2;
}

template <typename T, typename Cont>
bool
operator<  (queue<T, Cont> const& q1, queue<T, Cont> const& q2)
{
  return q1 < q2;
}

template <typename T, typename Cont>
bool
operator>  (queue<T, Cont> const& q1, queue<T, Cont> const& q2)
{
  return q1 > q2;
}

// --------------------------------------------------------------------------

}

#endif /* BOOST/QUEUE_H */
