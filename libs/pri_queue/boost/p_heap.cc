// -*-C++-*- boost/p_heap.cc
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
// Title:  Implementation of the pairing heap
// Version: $Id$

// --------------------------------------------------------------------------

#if !defined(BOOST_P_HEAP_CC)
#define BOOST_P_HEAP_CC 1

#if !defined(BOOST_P_HEAP_HPP)
#  include "boost/p_heap.hpp"
#endif

namespace boost
{

// --------------------------------------------------------------------------

template <typename T, typename Comp>
inline
pairing_heap<T, Comp>::node::node(T const& d):
  m_prev(0),
  m_children(0),
  m_data(d)
{
}

template <typename T, typename Comp>
inline void
pairing_heap<T, Comp>::node::insert(node** l, node* n)
{
  n->m_next = *l;
  if (n->m_next != 0)
    n->m_next->m_prev = &n->m_next;
  n->m_prev = l;
  *l = n;
}

template <typename T, typename Comp>
inline void
pairing_heap<T, Comp>::node::set_root(node** l, node* n)
{
  n->m_next = 0;
  n->m_prev = l;
  *l = n;
}

template <typename T, typename Comp>
inline void
pairing_heap<T, Comp>::node::remove(node* n)
{
  if (n->m_next != 0)
    n->m_next->m_prev = n->m_prev;
  *n->m_prev = n->m_next;
  n->m_prev = 0;
}

// --------------------------------------------------------------------------

template <typename T, typename Comp>
inline typename pairing_heap<T, Comp>::node*
pairing_heap<T, Comp>::link(node* n1, node* n2)
{
  if (m_compare(n2->data(), n1->data()))
    {
      node::insert(n1->children(), n2);
      return n1;
    }
  else
    {
      node::insert(n2->children(), n1);
      return n2;
    }
}

template <typename T, typename Comp>
inline typename pairing_heap<T, Comp>::node*
pairing_heap<T, Comp>::recursive_link(node* n1, node* n2, node* c)
{
  node::remove(n1);
  node::remove(n2);

  n1 = link(n1, n2);

  if (c)
    {
      if (c->next() != 0)
    c = recursive_link(c, c->next(), c->next()->next());
      else
    node::remove(c);
      n1 = link(n1, c);
    }

  return n1;
}

template <typename T, typename Comp>
inline void
pairing_heap<T, Comp>::unlink(node* n)
{
  node::remove(n);

  if (node* c = n->begin())
    {
      if (c->next() != 0)
    c = recursive_link(c, c->next(), c->next()->next());
      else
    node::remove(c);

      node::set_root(&m_root, m_root == 0? c: link(m_root, c));
    }
}

// --------------------------------------------------------------------------

template <typename T, typename Comp>
inline
pairing_heap<T, Comp>::pairing_heap(Comp const& comp):
  m_size(0),
  m_root(0),
  m_compare(comp)
{
}

template <typename T, typename Comp>
inline
pairing_heap<T, Comp>::~pairing_heap()
{
  iterator e = end();
  iterator it = begin();
  while (it != e)
    {
      node* del = it.m_queue.top();
      ++it;
      delete del;
    }
}

template <typename T, typename Comp>
inline typename pairing_heap<T, Comp>::pointer
pairing_heap<T, Comp>::push(T const& d)
{
  node* rc = new node(d);
  node::set_root(&m_root, m_size++ == 0? rc: link(rc, m_root));

  return rc;
}

template <typename T, typename Comp>
inline void
pairing_heap<T, Comp>::remove(pointer n)
{
  unlink(n);

  --m_size;
  delete n;
}

template <typename T, typename Comp>
inline void
pairing_heap<T, Comp>::pop()
{
  remove(m_root);
}

template <typename T, typename Comp>
template <typename K>
inline void
pairing_heap<T, Comp>::change(pointer n, K const& d)
{
  unlink(n);

  n->data(d);
  node::set_root(&m_root, m_root == 0? n: link(n, m_root));
}

template <typename T, typename Comp>
template <typename K>
inline void
pairing_heap<T, Comp>::change_top(K const& value)
{
  change(m_root, value);
}

template <typename T, typename Comp>
template <typename K>
inline void
pairing_heap<T, Comp>::decrease(pointer n, K const& d)
{
  change(n, d);
}

template <typename T, typename Comp>
template <typename K>
inline void
pairing_heap<T, Comp>::increase(pointer n, K const& d)
{
  change(n, d);
}

// --------------------------------------------------------------------------

} // namespace boost

#endif /* BOOST_P_HEAP_CC */
