// -*-C++-*- boost/s_heap.hpp
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
// Title:  A priority queue using a splay tree as representation
// Version: $Id$ 

// -------------------------------------------------------------------------- 

#if !defined(BOOST_S_HEAP_HPP)
#define BOOST_S_HEAP_HPP 1

#include <functional>
#if !defined(BOOST_STACK_HPP)
#  include "boost/stack.hpp"
#endif

namespace boost
{

// --------------------------------------------------------------------------

template <typename T, typename Comp = std::less<T> >
class splay_heap
{
  friend class const_iterator;
  struct node
  {
    node* m_left;
    node* m_right;
    node* m_next;
    T     m_data;

    node(T const& data): m_left(0), m_right(0), m_next(0), m_data(data) {}
    node* destroy()
      {
	if (m_left != 0)
	  delete m_left->destroy();
	if (m_right != 0)
	  delete m_right->destroy();
	if (m_next != 0)
	  delete m_next->destroy();
	
	return this;
      }
  };

public:
  typedef T             value_type;
  typedef T const&      const_reference;
  typedef unsigned long size_type;
  typedef Comp          compare_type;
  typedef node*         pointer;

  class const_iterator
  {
  public:
    const_iterator() {}
    explicit const_iterator(node* root) { m_stack.push(root); }
    // generator copy ctor, dtor, and copy assignment are appropriate

    bool operator== (const_iterator const& it) const { return m_stack.empty() == it.m_stack.empty(); }
    bool operator!= (const_iterator const& it) const { return m_stack.empty() != it.m_stack.empty(); }

    const_reference operator* () const { return m_stack.top()->m_data; }
    T const*        operator-> () const { return &(m_stack.top())->m_data; }

    const_iterator& operator++ ()
      {
	node* tmp = m_stack.top();
	m_stack.pop();
	if (tmp->m_left != 0)
          m_stack.push(tmp->m_left);
	if (tmp->m_right != 0)
          m_stack.push(tmp->m_right);
	return *this;
      }

    const_iterator operator++ (int)
      {
	const_iterator rc(*this);
	operator++();
	return rc;
      }

  private:
    stack<node*> m_stack;
  };

  splay_heap(Comp const& comp = Comp());
  ~splay_heap() { if (m_root) delete m_root->destroy(); }

  bool            empty() const { return m_size == 0; }
  size_type       size() const { return m_size; }
  const_reference top() const;
  pointer         push(const_reference);
  void            pop();

  template <typename K> void change_top(K const&);
  template <typename K> void change(pointer, K const&);
  template <typename K> void decrease(pointer, K const&);
  template <typename K> void increase(pointer, K const&);
  void remove(pointer);

  const_iterator begin() const { return const_iterator(m_root); }
  const_iterator end() const { return const_iterator(); }

private:
  void insert(node*) const;
  void detach(node*);
  static void splay_min(node*&);
  static void splay_max(node*&);

  Comp          m_compare;
  size_type     m_size;
  mutable node* m_root;

  splay_heap(splay_heap const&);     // deliberately not implemented
  void operator=(splay_heap const&); // deliberately not implemented
};

// --------------------------------------------------------------------------

} // namespace boost

#if !defined(BOOST_S_HEAP_CC)
#  include "boost/s_heap.cc"
#endif

#endif /* BOOST_S_HEAP_HPP */
