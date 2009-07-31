// -*-C++-*- boost/s_heap.cc
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
// Title:  Implementation of the priority queue using splay trees
// Version: $Id$ 

// -------------------------------------------------------------------------- 

#if !defined(BOOST_S_HEAP_CC)
#define BOOST_S_HEAP_CC 1

#if !defined(BOOST_S_HEAP_HPP)
#  include "boost/s_heap.hpp"
#endif

namespace boost
{

template <typename T, typename Comp>
inline
splay_heap<T, Comp>::splay_heap(Comp const& comp):
  m_compare(comp),
  m_size(0),
  m_root(0)
{
}

// --------------------------------------------------------------------------

template <typename T, typename Comp>
inline void
splay_heap<T, Comp>::insert(node* n) const
{
  node** child_pointer = &m_root; // pointer to the [potential] pointer to the node

  while (*child_pointer != 0)
    if (m_compare((*child_pointer)->m_data, n->m_data))
      child_pointer = &((*child_pointer)->m_right);
    else if (m_compare(n->m_data, (*child_pointer)->m_data))
      child_pointer = &((*child_pointer)->m_left);
    else
      {
	n->m_next = (*child_pointer)->m_next;
	(*child_pointer)->m_next = n;
	return;
      }

  *child_pointer = n;
}

// --------------------------------------------------------------------------
// The splay heap only splays to get access to the max node! Thus, only
// zig-zig and, potentially, zig operations are needed.

template <typename T, typename Comp>
inline void
splay_heap<T, Comp>::splay_max(node*& child_pointer)
{
  node*  root = 0;
  node** insert_point = &root;

  while (child_pointer->m_right)
    {
      node* tmp = child_pointer->m_right;
      if (tmp->m_right != 0)
	{ // zig-zig
	  *insert_point = tmp;
	  child_pointer->m_right = tmp->m_left;
	  tmp->m_left = child_pointer;
	  child_pointer = tmp->m_right;
	  tmp->m_right = 0;
	  insert_point = &tmp->m_right;
	}
      else
	{ // zig
          *insert_point = child_pointer;
          child_pointer->m_right = 0;
	  insert_point = &child_pointer->m_right;
          child_pointer = tmp;
	}
    }

  if (root != 0)
    {
      *insert_point = child_pointer->m_left;
      child_pointer->m_left = root;
    }
}

// --------------------------------------------------------------------------

template <typename T, typename Comp>
inline void
splay_heap<T, Comp>::detach(node* n)
{
  node** child_pointer = &m_root; // pointer to the [potential] pointer to the node

  while (true)
    if (m_compare((*child_pointer)->m_data, n->m_data))
      child_pointer = &((*child_pointer)->m_right);
    else if (m_compare(n->m_data, (*child_pointer)->m_data))
      child_pointer = &((*child_pointer)->m_left);
    else
      break;

  node* tmp = *child_pointer;

  if (tmp->m_next == 0)
    { // the node is really cut...
      if (tmp->m_left != 0)
	{
	  splay_max(tmp->m_left);
	  *child_pointer = tmp->m_left;
	  (*child_pointer)->m_right = tmp->m_right;
	  tmp->m_left = tmp->m_right = 0;
	}
      else
	{
	  *child_pointer = tmp->m_right;
	  tmp->m_right = 0;
	}
    }
  else
    { // it is just replaced by another one with the same priority
      if (tmp == n)
	{ // the sought node is just the head of the list
	  *child_pointer = tmp->m_next;
	  (*child_pointer)->m_left = tmp->m_left;
	  (*child_pointer)->m_right = tmp->m_right;
	  tmp->m_next = tmp->m_left = tmp->m_right = 0;
	}
      else
	{ // the sought node is somewhere down the list
          for (child_pointer = &tmp->m_next; (*child_pointer) != n; )
	    child_pointer = &((*child_pointer)->m_next);
	  
	  tmp = *child_pointer;
	  (*child_pointer) = tmp->m_next;
	  tmp->m_next = 0;
	}
    }
}

// --------------------------------------------------------------------------

template <typename T, typename Comp>
inline typename splay_heap<T, Comp>::pointer
splay_heap<T, Comp>::push(const_reference val)
{
  ++m_size;
  node* rc = new node(val);
  insert(rc);
  return rc;
}

template <typename T, typename Comp>
inline typename splay_heap<T, Comp>::const_reference
splay_heap<T, Comp>::top() const
{
  splay_max(m_root);
  return m_root->m_data;
}

template <typename T, typename Comp>
inline void
splay_heap<T, Comp>::pop()
{
  splay_max(m_root);
  node* del = m_root;
  if (del->m_next != 0)
    {
      m_root = del->m_next;
      m_root->m_left = del->m_left;
      m_root->m_right = del->m_right;
    }
  else
    m_root = del->m_left;
  --m_size;
  delete del;
}

// --------------------------------------------------------------------------

template <typename T, typename Comp>
template <typename K>
inline void
splay_heap<T, Comp>::change_top(K const& val)
{
  // TODO: test possible improvement: if the new priority is larger, just the
  //       value has to be changed.

  splay_max(m_root);
  node* tmp = m_root;
  if (tmp->m_next != 0)
    {
      m_root = tmp->m_next;
      m_root->m_left = tmp->m_left;
      m_root->m_right = tmp->m_right;
    }
  else
    m_root = tmp->m_left;
  tmp->m_data = val;
  insert(tmp);
}

// --------------------------------------------------------------------------

template <typename T, typename Comp>
template <typename K>
inline void
splay_heap<T, Comp>::change(pointer n, K const& val)
{
  detach(n);
  n->m_data = val;
  insert(n);
}

template <typename T, typename Comp>
template <typename K>
inline void
splay_heap<T, Comp>::decrease(pointer n, K const& val)
{
  change(n, val);
}

template <typename T, typename Comp>
template <typename K>
inline void
splay_heap<T, Comp>::increase(pointer n, K const& val)
{
  change(n, val);
}

template <typename T, typename Comp>
inline void
splay_heap<T, Comp>::remove(pointer n)
{
  detach(n);
  delete n;
}

// --------------------------------------------------------------------------

} // namespace boost

#endif /* BOOST_S_HEAP_CC */
