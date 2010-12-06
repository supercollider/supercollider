// -*-C++-*- boost/stack.h
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
// Title:  A stack adaptor with extended functionality compared to the standard one
// Version: $Id$ 

// -------------------------------------------------------------------------- 

#if !defined(BOOST_STACK_H)
#define BOOST_STACK_H 1

#include <vector>

namespace boost
{

template <typename T, typename Cont = std::vector<T> >
class stack;

template <typename T, typename Cont>
bool operator== (stack<T, Cont> const&, stack<T, Cont> const&);
template <typename T, typename Cont>
bool operator!= (stack<T, Cont> const&, stack<T, Cont> const&);
template <typename T, typename Cont>
bool operator<= (stack<T, Cont> const&, stack<T, Cont> const&);
template <typename T, typename Cont>
bool operator>= (stack<T, Cont> const&, stack<T, Cont> const&);
template <typename T, typename Cont>
bool operator<  (stack<T, Cont> const&, stack<T, Cont> const&);
template <typename T, typename Cont>
bool operator>  (stack<T, Cont> const&, stack<T, Cont> const&);


// --------------------------------------------------------------------------

template <typename T, typename Cont>
class stack
{
  friend bool operator== <T, Cont>(stack<T, Cont> const&, stack<T, Cont> const&);
  friend bool operator!= <T, Cont>(stack<T, Cont> const&, stack<T, Cont> const&);
  friend bool operator<= <T, Cont>(stack<T, Cont> const&, stack<T, Cont> const&);
  friend bool operator>= <T, Cont>(stack<T, Cont> const&, stack<T, Cont> const&);
  friend bool operator<  <T, Cont>(stack<T, Cont> const&, stack<T, Cont> const&);
  friend bool operator>  <T, Cont>(stack<T, Cont> const&, stack<T, Cont> const&);

public:
  typedef typename Cont::value_type      value_type;
  typedef typename Cont::reference       reference;
  typedef typename Cont::const_reference const_reference;
  typedef typename Cont::size_type       size_type;
  typedef Cont                           container_type;
  typedef typename Cont::iterator        iterator;
  typedef typename Cont::const_iterator  const_iterator;

  explicit stack(Cont const& cont = Cont()): m_container(cont) {}

  bool              empty() const               { return m_container.empty(); }
  size_type         size() const                { return m_container.size(); }
  void              push(value_type const& val) { m_container.push_back(val); }
  void              pop()                       { m_container.pop_back(); }
  value_type const& top() const                 { return m_container.back(); }
  value_type&       top()                       { return m_container.back(); }

  iterator       begin()       { return m_container.begin(); }
  iterator       end()         { return m_container.end(); }
  const_iterator begin() const { return m_container.begin(); }
  const_iterator end() const   { return m_container.end(); }

private:
  Cont m_container;
};

template <typename T, typename Cont>
bool
operator== (stack<T, Cont> const& s1, stack<T, Cont> const&s2)
{
  return s1 == s2;
}

template <typename T, typename Cont>
bool
operator!= (stack<T, Cont> const& s1, stack<T, Cont> const& s2)
{
  return s1 != s2;
}

template <typename T, typename Cont>
bool
operator<= (stack<T, Cont> const& s1, stack<T, Cont> const& s2)
{
  return s1 <= s2;
}

template <typename T, typename Cont>
bool
operator>= (stack<T, Cont> const& s1, stack<T, Cont> const& s2)
{
  return s1 >= s2;
}

template <typename T, typename Cont>
bool
operator<  (stack<T, Cont> const& s1, stack<T, Cont> const& s2)
{
  return s1 < s2;
}

template <typename T, typename Cont>
bool
operator>  (stack<T, Cont> const& s1, stack<T, Cont> const& s2)
{
  return s1 > s2;
}

// --------------------------------------------------------------------------

}

#endif /* BOOST_STACK_H */
