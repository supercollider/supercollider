// -*-C++-*- boost/d_heap.hpp
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
// Title:  Declarations for the d-heap implementation
// Version: $Id$

// --------------------------------------------------------------------------

#if !defined(BOOST_D_HEAP_HPP)
#define BOOST_D_HEAP_HPP 1

#include <functional>
#include <list>

namespace boost
{

    // --------------------------------------------------------------------------
    // d-heap: A heap using d nodes as children

    template <typename T>
    class d_heap_base
    {
    protected:
        struct node;
    public:
        typedef T                                   value_type;
        typedef T&                                  reference;
        typedef T const&                            const_reference;
        typedef typename std::list<struct node>::size_type size_type;

    protected:
        struct node
        {
            size_type m_index;
            T         m_value;
            node(size_type idx, T const& value): m_index(idx), m_value(value) {}
        };

    private:
        typedef typename std::list<struct node>::iterator  node_iterator;

    public:

        class const_iterator
        {
        public:
            const_iterator() {}
            const_iterator(typename std::vector<node_iterator>::const_iterator it): m_it(it) {}

            T const&        operator* () const { return (*m_it)->m_value; }
            T const*        operator-> () const { return &(*m_it)->m_value; }
            const_iterator& operator++ () { ++m_it; return *this; }
            const_iterator  operator++ (int) { const_iterator rc(*this); operator++(); return rc; }

            bool operator== (const_iterator const& it) const { return m_it == it.m_it; }
            bool operator!= (const_iterator const& it) const { return m_it != it.m_it; }

        private:
            typename std::vector<node_iterator>::const_iterator m_it;
        };
        typedef const_iterator iterator;

        d_heap_base() {}
        const_reference top() const   { return m_heap[0]->m_value; }
        bool            empty() const { return m_nodes.empty(); }
        size_type       size() const  { return m_nodes.size(); }
        const_iterator  begin() const { return const_iterator(m_heap.begin()); }
        const_iterator  end() const   { return const_iterator(m_heap.end()); }

    protected:
        ~d_heap_base() {}

        std::list<node>            m_nodes;
        std::vector<node_iterator> m_heap;

    private:
        d_heap_base(d_heap_base const&);    // deliberately not implemented
        void operator=(d_heap_base const&); // deliberately not implemented
    };

    // -----------------------------------------------------------------------------

    template <typename T, typename Comp = std::less<T>, int d = 2>
    class d_heap: public d_heap_base<T>
    {
    public:
        typedef typename d_heap_base<T>::node node;
        typedef typename std::list<node>::iterator  node_iterator;

    public:
        typedef typename std::list<node>::iterator  pointer;

        typedef T                                   value_type;
        typedef T&                                  reference;
        typedef T const&                            const_reference;
        typedef typename d_heap<T>::const_iterator  const_iterator;
        typedef typename std::list<node>::size_type size_type;
        typedef Comp                                compare_type;

        explicit d_heap(Comp const& comp = Comp()):  m_compare(comp){}

        pointer push(const_reference value);

        void    pop();

        template <typename K> void change_top(K const& data);

        template <typename K> void change(pointer, K const&);
        template <typename K> void decrease(pointer, K const&);
        template <typename K> void increase(pointer, K const&);
        void    remove(pointer);

    private:
        void siftup(size_type, pointer);

        Comp m_compare;
    };

    // --------------------------------------------------------------------------

} // boost

#if !defined(BOOST_D_HEAP_CC)
#  include "boost/d_heap.cc"
#endif

#endif /* BOOST_D_HEAP_HPP */
