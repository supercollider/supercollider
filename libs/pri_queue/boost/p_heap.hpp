// -*-C++-*- boost/p_heap.hpp
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
// Title:  declarations for an implementation of pairing heaps
// Version: $Id$

// --------------------------------------------------------------------------

#if !defined(BOOST_P_HEAP_HPP)
#define BOOST_P_HEAP_HPP 1

#include <functional>
#if !defined(BOOST_QUEUE_HPP)
#  include "boost/queue.hpp"
#endif

namespace boost
{

// --------------------------------------------------------------------------

template <typename T, typename Comp = std::less<T> >
class pairing_heap
{
private:
  class node
  {
  public:
    explicit node(T const& d);

    static void insert(node** l, node* n);
    static void set_root(node** l, node* n);
    static void remove(node* n);

    T const& data() const     { return m_data; }
    template <typename K> void data(K const& d) { m_data = d; }

    node** children() { return &m_children; }
    node*  begin()    { return m_children; }
    node*  next()     { return m_next; }
    node** prev() { return m_prev; }

  private:
    node** m_prev;
    node*  m_next;
    node*  m_children;
    T      m_data;
  };

public:
  class iterator
  {
    friend class pairing_heap;
    explicit iterator(node* root) { if (root != 0) m_queue.push(root); }

  public:
    iterator() {}

    T const& operator* () const { return m_queue.top()->data(); }
    T const* operator->() const { return &m_queue.top()->data(); }

    iterator& operator++()
      {
    node* tmp = m_queue.top()->begin();
    m_queue.pop();
    for (; tmp != 0; tmp = tmp->next())
      m_queue.push(tmp);
    return *this;
      }
    iterator  operator++(int)
      {
    iterator rc(*this);
    operator++();
    return rc;
      }

    bool operator== (iterator const& it) const
      {
    return m_queue.empty() == it.m_queue.empty()
           && (m_queue.empty() || &m_queue.top() == &it.m_queue.top());
      }
    bool operator!= (iterator const& it) const { return !operator== (it); }

  private:
    queue<node*> m_queue;
  };

  typedef T             value_type;
  typedef T&            reference;
  typedef T const&      const_reference;
  typedef iterator      const_iterator;
  typedef unsigned long size_type;
  typedef node*         pointer;

  explicit pairing_heap(Comp const& comp = Comp());
  ~pairing_heap();

  bool      empty() const { return m_size == 0; }
  size_type size() const  { return m_size; }
  pointer   push(T const& d);
  T const&  top() const { return m_root->data(); }
  void      pop();

  iterator  begin() const { return iterator(m_root); }
  iterator  end() const { return iterator(); }

  void      remove(pointer);
  template <typename K> void change(pointer, K const&);
  template <typename K> void change_top(K const&);
  template <typename K> void increase(pointer, K const&);
  template <typename K> void decrease(pointer, K const&);

private:
  node* link(node* n1, node* n2);
  node* recursive_link(node* n1, node* n2, node* tail);
  void  unlink(node*);

  size_type m_size;
  node*     m_root;
  Comp      m_compare;

  pairing_heap(pairing_heap const&); // deliberately not implemented
  void operator= (pairing_heap const&); // deliberately not implemented
};

// --------------------------------------------------------------------------

}

#if !defined(BOOST_P_HEAP_CC)
#  include "boost/p_heap.cc"
#endif

#endif /* BOOST_P_HEAP_H */
