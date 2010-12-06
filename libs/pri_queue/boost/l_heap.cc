// -*-C++-*- boost/l_heap.cc
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
// Title:  Implementation of the lazy Fibonacci heap variant
// Version: $Id$

// --------------------------------------------------------------------------

#if !defined(BOOST_L_HEAP_CC)
#define BOOST_L_HEAP_CC 1

#if !defined(BOOST_L_HEAP_HPP)
#  include "boost/l_heap.hpp"
#endif

namespace boost
{

// --- implementation of the iterator class for lazy Fibonacci heaps -------------

template <typename T>
inline typename lazy_fibonacci_heap_base<T>::node const*
lazy_fibonacci_heap_base<T>::iterator::find_root(typename std::vector<node*>::const_iterator it) const
{
  for (; it != m_heap->m_roots.end(); ++it)
    if ((*it) != 0)
      return (*it);
  return 0;
}

template <typename T>
inline typename lazy_fibonacci_heap_base<T>::node const*
lazy_fibonacci_heap_base<T>::iterator::find_leaf(node const* n) const
{
  if (n == 0)
    return 0;

  while (n->m_children.size() != 0)
    n = n->m_children[0];

  return n;
}

template <typename T>
inline
lazy_fibonacci_heap_base<T>::iterator::iterator(lazy_fibonacci_heap_base<T> const* h):
  m_heap(h),
  m_node(0)
{
  if (m_heap->m_size == 0)
    return;

  m_node = find_leaf(find_root(m_heap->m_roots.begin()));
}

template <typename T>
inline typename lazy_fibonacci_heap_base<T>::iterator&
lazy_fibonacci_heap_base<T>::iterator::operator++ ()
{
  if (m_node->state() == clean)
    m_node = find_leaf(find_root(m_heap->m_roots.begin() + m_node->rank() + 1));
  else if (m_node->m_parent->m_children.size() == m_node->m_index + 1)
    m_node = m_node->m_parent;
  else
    m_node = find_leaf(m_node->m_parent->m_children[m_node->m_index + 1]);

  return *this;
}

template <typename T>
inline typename  lazy_fibonacci_heap_base<T>::iterator
lazy_fibonacci_heap_base<T>::iterator::operator++ (int)
{
  iterator rc(*this);
  operator++ ();
  return rc;
}

// --- implementation of Fibonacci heap's node member functions ----------------

template <typename T>
inline typename  lazy_fibonacci_heap_base<T>::node*
lazy_fibonacci_heap_base<T>::node::join(node* tree)
{
  tree->m_index  = m_children.size();
  tree->m_parent = this;
  m_children.push_back(tree);
  m_state = clean;
  tree->m_state = cut_none;

  return this;
}

template <typename T>
inline void
lazy_fibonacci_heap_base<T>::node::cut(node* child)
{
  size_type index = child->m_index;
  if (m_children.size() > index + 1)
    {
      m_children[index] = m_children[m_children.size() - 1];
      m_children[index]->m_index = index;
    }
  m_children.pop_back();
  m_state = node_state(m_state + 1);
  // m_state = static_cast<node_state>(m_state + 1);
}

template <typename T>
inline void
lazy_fibonacci_heap_base<T>::node::destroy()
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
lazy_fibonacci_heap_base<T>::~lazy_fibonacci_heap_base()
{
  typename std::vector<node*>::size_type idx = m_roots.size();
  while (idx-- != 0)
    if (m_roots[idx] != 0)
    {
      m_roots[idx]->destroy();
      delete m_roots[idx];
    }
  idx = m_cut_nodes.size();
  while (idx-- != 0)
    {
      m_cut_nodes[idx]->destroy();
      delete m_cut_nodes[idx];
    }
}

// --- implementation of auxiliary the Fibonacci heap member functions ---------

template <typename T, typename Comp>
inline void
lazy_fibonacci_heap<T, Comp>::add_root(node* n) const
{
  size_type rank = n->rank();
  if (lazy_fibonacci_heap_base<T>::m_roots.size() <= rank)
    {
      while (lazy_fibonacci_heap_base<T>::m_roots.size() < rank)
        lazy_fibonacci_heap_base<T>::m_roots.push_back(0);
      lazy_fibonacci_heap_base<T>::m_roots.push_back(n);
      n->clear();
    }
  else if (lazy_fibonacci_heap_base<T>::m_roots[rank] == 0)
    {
      lazy_fibonacci_heap_base<T>::m_roots[rank] = n;
      n->clear();
    }
  else
    {
      node* r = lazy_fibonacci_heap_base<T>::m_roots[rank];
      r->set_node();
      lazy_fibonacci_heap_base<T>::m_roots[rank] = 0;
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
lazy_fibonacci_heap<T, Comp>::find_min() const
{
  if (lazy_fibonacci_heap_base<T>::m_size == 0)
    m_min = 0;
  else
    {
      typename std::vector<node*>::const_iterator end = lazy_fibonacci_heap_base<T>::m_cut_nodes.end();
      typename std::vector<node*>::const_iterator it = lazy_fibonacci_heap_base<T>::m_cut_nodes.begin();

      for (; it != end; ++it)
    add_root(*it);
      lazy_fibonacci_heap_base<T>::m_cut_nodes.erase(lazy_fibonacci_heap_base<T>::m_cut_nodes.begin(), lazy_fibonacci_heap_base<T>::m_cut_nodes.end());

      end = lazy_fibonacci_heap_base<T>::m_roots.end();
      it = lazy_fibonacci_heap_base<T>::m_roots.begin();
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
lazy_fibonacci_heap<T, Comp>::cut(node* n)
{
  node* p = n->parent();
  p->cut(n);
  switch (p->state())
    {
    case lazy_fibonacci_heap_base<T>::clean:
      assert("clean after loosing a child!" == 0);
      break;
    case lazy_fibonacci_heap_base<T>::root_cut_one:
      lazy_fibonacci_heap_base<T>::m_roots[p->rank() + 1] = 0;
      p->set_cut();
      lazy_fibonacci_heap_base<T>::m_cut_nodes.push_back(p);
      m_min = 0;
      break;
    case lazy_fibonacci_heap_base<T>::cut_one:
      // nothing to do
      break;
    case lazy_fibonacci_heap_base<T>::cut_two:
      cut(p);
      p->set_cut();
      lazy_fibonacci_heap_base<T>::m_cut_nodes.push_back(p);
      break;
    default:
      // we are in the cut state...
      break;
    }
}

// --- implementation of Fibonacci heap's modifying member functions -----------

template <typename T, typename Comp>
template <typename K>
inline void
lazy_fibonacci_heap<T, Comp>::decrease(pointer n, K const& data)
{
  n->data(data);
  typename std::vector<node*>::const_iterator it = n->begin();
  typename std::vector<node*>::const_iterator end = n->end();
  for (; it != end; ++it)
    if (m_compare(n->data(), (*it)->data()))
      {
    if (n->state() == lazy_fibonacci_heap_base<T>::clean)
      lazy_fibonacci_heap_base<T>::m_roots[n->rank()] = 0;
    else
      cut(n);

    for (it = n->begin(); it != end; ++it)
      {
        (*it)->set_cut();
        lazy_fibonacci_heap_base<T>::m_cut_nodes.push_back(*it);
      }
    n->remove_all();

    lazy_fibonacci_heap_base<T>::m_cut_nodes.push_back(*it);
    break;
      }

  if (m_min == n)
    m_min = 0;
}

template <typename T, typename Comp>
template <typename K>
inline void
lazy_fibonacci_heap<T, Comp>::increase(pointer n, K const& data)
{
  n->data(data);
  if (n->state() < lazy_fibonacci_heap_base<T>::clean && m_compare(n->parent()->data(), n->data()))
    {
      cut(n);
      lazy_fibonacci_heap_base<T>::m_cut_nodes.push_back(n);
      m_min = 0;
    }
}

template <typename T, typename Comp>
template <typename K>
inline void
lazy_fibonacci_heap<T, Comp>::change(pointer n, K const& data)
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
lazy_fibonacci_heap<T, Comp>::change_top(K const& data)
{
  if (m_min == 0)
    find_min();
  change(m_min, data);
}

template <typename T, typename Comp>
inline void
lazy_fibonacci_heap<T, Comp>::remove(pointer n)
{
  switch (n->state())
    {
    case lazy_fibonacci_heap_base<T>::cut_self:
      {
    for (typename std::vector<node*>::iterator it = lazy_fibonacci_heap_base<T>::m_cut_nodes.begin(); it != lazy_fibonacci_heap_base<T>::m_cut_nodes.end(); ++it)
      if (*it == n)
        {
          *it = lazy_fibonacci_heap_base<T>::m_cut_nodes[lazy_fibonacci_heap_base<T>::m_cut_nodes.size() - 1];
          lazy_fibonacci_heap_base<T>::m_cut_nodes.pop_back();
          break;
        }
      }
      break;
    case lazy_fibonacci_heap_base<T>::clean: lazy_fibonacci_heap_base<T>::m_roots[n->rank()] = 0; break;
    default: cut(n); break;
    }

  typename std::vector<node*>::const_iterator it = n->begin();
  typename std::vector<node*>::const_iterator end = n->end();
  for (it = n->begin(); it != end; ++it)
    {
      (*it)->set_cut();
      lazy_fibonacci_heap_base<T>::m_cut_nodes.push_back(*it);
    }

  m_min = 0;
  delete n;
}

// --- implementation of basic heap interface for Fibonacci heaps --------------

template <typename T, typename Comp>
inline T const&
lazy_fibonacci_heap<T, Comp>::top() const
{
  if (m_min == 0)
    find_min();
  return m_min->data();
}

template <typename T, typename Comp>
inline typename lazy_fibonacci_heap<T, Comp>::pointer
lazy_fibonacci_heap<T, Comp>::push(T const& data)
{
  node* n = new node(data);
  ++lazy_fibonacci_heap_base<T>::m_size;
  n->set_cut();
  lazy_fibonacci_heap_base<T>::m_cut_nodes.push_back(n);

  m_min = 0;
  return n;
}

template <typename T, typename Comp>
inline void
lazy_fibonacci_heap<T, Comp>::pop()
{
  if (m_min == 0)
    find_min();

  node* del = m_min;
  lazy_fibonacci_heap_base<T>::m_roots[m_min->rank()] = 0;

  typename std::vector<node*>::const_iterator end = del->end();

  for (typename std::vector<node*>::const_iterator it = del->begin(); it != end; ++it)
    {
      (*it)->set_cut();
      lazy_fibonacci_heap_base<T>::m_cut_nodes.push_back(*it);
    }
  --lazy_fibonacci_heap_base<T>::m_size;

  delete del;
  m_min = 0;
}


// --------------------------------------------------------------------------

} // namespace boost

#endif /* BOOST_L_HEAP_CC */
