// -*-C++-*- boost/f_heap.cc
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
// Title:  Implementation of a Fibonacci heap
// Version: $Id$

// --------------------------------------------------------------------------

#if !defined(BOOST_F_HEAP_CC)
#define BOOST_F_HEAP_CC 1

#if !defined(BOOST_F_HEAP_HPP)
#  include "f_heap.hpp"
#endif

namespace boost
{

// --- implementation of the iterator class for Fibonacci heaps -------------

template <typename T>
inline typename fibonacci_heap_base<T>::node const*
fibonacci_heap_base<T>::iterator::find_root(typename std::vector<node*>::const_iterator it) const
{
  for (; it != m_heap->m_roots.end(); ++it)
    if ((*it) != 0)
      return (*it);
  return 0;
}

template <typename T>
inline typename fibonacci_heap_base<T>::node const*
fibonacci_heap_base<T>::iterator::find_leaf(node const* n) const
{
  if (n == 0)
    return 0;

  while (n->m_children.size() != 0)
    n = n->m_children[0];

  return n;
}

template <typename T>
inline
fibonacci_heap_base<T>::iterator::iterator(fibonacci_heap_base<T> const* h):
  m_heap(h),
  m_node(0)
{
  if (m_heap->m_size == 0)
    return;

  m_node = find_leaf(find_root(m_heap->m_roots.begin()));
}

template <typename T>
inline typename fibonacci_heap_base<T>::iterator&
fibonacci_heap_base<T>::iterator::operator++ ()
{
  if (m_node->is_root())
    m_node = find_leaf(find_root(m_heap->m_roots.begin() + m_node->rank() + 1));
  else if (m_node->m_parent->m_children.size() == m_node->m_index + 1)
    m_node = m_node->m_parent;
  else
    m_node = find_leaf(m_node->m_parent->m_children[m_node->m_index + 1]);

  return *this;
}

template <typename T>
inline typename fibonacci_heap_base<T>::iterator
fibonacci_heap_base<T>::iterator::operator++ (int)
{
  iterator rc(*this);
  operator++ ();
  return rc;
}

// --- implementation of Fibonacci heap's node member functions ----------------

template <typename T>
inline typename fibonacci_heap_base<T>::node*
fibonacci_heap_base<T>::node::join(node* tree)
{
  tree->m_index  = m_children.size();
  tree->m_parent = this;
  m_children.push_back(tree);
  m_lost_child = 0;

  return this;
}

template <typename T>
inline void
fibonacci_heap_base<T>::node::cut(node* child)
{
  size_type index = child->m_index;
  if (m_children.size() > index + 1)
    {
      m_children[index] = m_children[m_children.size() - 1];
      m_children[index]->m_index = index;
    }
  m_children.pop_back();
  ++m_lost_child;
}

template <typename T>
inline void
fibonacci_heap_base<T>::node::destroy()
{
  typename std::vector<node*>::size_type idx = m_children.size();
  while (idx-- != 0)
    if (m_children[idx] != 0)
      {
    m_children[idx]->destroy();
    delete m_children[idx];
      }
}

template <typename T>
inline
fibonacci_heap_base<T>::~fibonacci_heap_base()
{
  if (m_size == 0)
    return;

  typename std::vector<node*>::size_type idx = m_roots.size();
  while (idx-- != 0)
    if (m_roots[idx] != 0)
      {
    m_roots[idx]->destroy();
    delete m_roots[idx];
      }
}

// --- implementation of auxiliary the Fibonacci heap member functions ---------

template <typename T, typename Comp>
inline void
fibonacci_heap<T, Comp>::add_root(node* n)
{
  size_type rank = n->rank();
  if (fibonacci_heap_base<T>::m_roots.size() <= rank)
    {
      while (fibonacci_heap_base<T>::m_roots.size() < rank)
        fibonacci_heap_base<T>::m_roots.push_back(0);
      fibonacci_heap_base<T>::m_roots.push_back(n);
      n->clear();
    }
  else if (fibonacci_heap_base<T>::m_roots[rank] == 0)
    {
      fibonacci_heap_base<T>::m_roots[rank] = n;
      n->clear();
    }
  else
    {
      node* r = fibonacci_heap_base<T>::m_roots[rank];
      fibonacci_heap_base<T>::m_roots[rank] = 0;
      if (m_compare(n->data(), r->data()))
    {
      n->clear();
      add_root(r->join(n));
    }
      else
    {
      r->clear();
      add_root(n->join(r));
    }
    }
}

template <typename T, typename Comp>
inline void
fibonacci_heap<T, Comp>::find_min() const
{
  if (fibonacci_heap_base<T>::m_size == 0)
      m_min = 0;
  else
    {
      typename std::vector<node*>::const_iterator end = fibonacci_heap_base<T>::m_roots.end();
      typename std::vector<node*>::const_iterator it = fibonacci_heap_base<T>::m_roots.begin();
      while (*it == 0)
        ++it;
      m_min = *it;

      for (++it; it != end; ++it)
        if (*it != 0 && m_compare(m_min->data(), (*it)->data()))
          m_min = *it;
    }
}

template <typename T, typename Comp>
inline void
fibonacci_heap<T, Comp>::cut(node* n)
{
  node* p = n->parent();
  p->cut(n);
  if (p->is_root())
    {
      fibonacci_heap_base<T>::m_roots[p->rank() + 1] = 0;
      add_root(p);
    }
  else if (p->lost_child() == 2)
    {
      cut(p);
      add_root(p);
    }
}

// --- implementation of Fibonacci heap's modifying member functions -----------

template <typename T, typename Comp>
template <typename K>
inline void
fibonacci_heap<T, Comp>::decrease(pointer n, K const& data)
{
  n->data(data);
  typename std::vector<node*>::const_iterator it = n->begin();
  typename std::vector<node*>::const_iterator end = n->end();
  for (; it != end; ++it)
    if (m_compare(n->data(), (*it)->data()))
      {
    if (n->is_root())
      fibonacci_heap_base<T>::m_roots[n->rank()] = 0;
    else
      cut(n);

    for (it = n->begin(); it != end; ++it)
      add_root(*it);
    n->remove_all();

    add_root(n);
    break;
      }
  m_min = 0;
}

template <typename T, typename Comp>
template <typename K>
inline void
fibonacci_heap<T, Comp>::increase(pointer n, K const& data)
{
  n->data(data);
  if (!n->is_root() && m_compare(n->parent()->data(), n->data()))
    {
      cut(n);
      add_root(n);
    }
  m_min = 0;
}

template <typename T, typename Comp>
template <typename K>
inline void
fibonacci_heap<T, Comp>::change(pointer n, K const& data)
{
  T comp(n->data());
  comp = data;

  if (m_compare(n->data(), comp))
    increase(n, data);
  else
    decrease(n, data);
}

template <typename T, typename Comp>
template <typename K>
inline void
fibonacci_heap<T, Comp>::change_top(K const& data)
{
  if (m_min == 0)
    find_min();
  change(m_min, data);
}

template <typename T, typename Comp>
inline void
fibonacci_heap<T, Comp>::remove(pointer n)
{
  if (n->is_root())
    fibonacci_heap_base<T>::m_roots[n->rank()] = 0;
  else
    cut(n);

  typename std::vector<node*>::const_iterator it = n->begin();
  typename std::vector<node*>::const_iterator end = n->end();
  for (it = n->begin(); it != end; ++it)
    add_root(*it);

  m_min = 0;
  delete n;
}

// --- implementation of basic heap interface for Fibonacci heaps --------------

template <typename T, typename Comp>
inline T const&
fibonacci_heap<T, Comp>::top() const
{
  if (m_min == 0)
    find_min();
  return m_min->data();
}

template <typename T, typename Comp>
inline typename fibonacci_heap<T, Comp>::pointer
fibonacci_heap<T, Comp>::push(T const& data)
{
  node* n = new node(data);
  ++fibonacci_heap_base<T>::m_size;
  add_root(n);

  m_min = 0;
  return n;
}

template <typename T, typename Comp>
inline void
fibonacci_heap<T, Comp>::pop()
{
  if (m_min == 0)
    find_min();

  node* del = m_min;
  fibonacci_heap_base<T>::m_roots[m_min->rank()] = 0;

  typename std::vector<node*>::const_iterator end = del->end();

  for (typename std::vector<node*>::const_iterator it = del->begin(); it != end; ++it)
    add_root(*it);
  --fibonacci_heap_base<T>::m_size;

  delete del;
  m_min = 0;
}

// -----------------------------------------------------------------------------

} // namespace boost

#endif /* BOOST_F_HEAP_CC */
