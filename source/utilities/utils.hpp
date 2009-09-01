//  $Id$
//
//  utility functions
//  Copyright (C) 2005, 2006, 2007  Tim Blechmann
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

//  $Revision$
//  $LastChangedRevision$
//  $LastChangedDate$
//  $LastChangedBy$

#ifndef _UTILS_HPP
#define _UTILS_HPP

#include <cstddef>
#include <cassert>
#include <algorithm>
#include <memory>
#include <list>
#include <iterator>
#include <valarray>

#include "boost/bind.hpp"
#include "boost/mem_fn.hpp"
#include "boost/functional.hpp"
#include "boost/array.hpp"

/** \todo later use tr1::hash */
#include "boost/functional/hash.hpp"

/** \todo later use std::tr1 type_traits */
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/noncopyable.hpp"

/** \todo later use std::tr1 smart pointers */
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/detail/atomic_count.hpp>

#include "branch_hints.hpp"

#if 0
#include <google/dense_hash_map>
#include <google/dense_hash_set>
#endif

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

typedef unsigned int uint;

#include "malloc_aligned.hpp"

namespace nova
{
/* we're using some member of the namespaces std and boost */
using std::auto_ptr;
using std::for_each;
using boost::mem_fun;
using boost::mem_fun_ref;
using boost::bind;
using std::swap;
using boost::tuple;
using boost::tie;
using boost::make_tuple;
using boost::noncopyable;

using boost::shared_ptr;
using boost::weak_ptr;
using boost::scoped_ptr;
using boost::intrusive_ptr;
using boost::dynamic_pointer_cast;
using boost::static_pointer_cast;

#if 0                           /* currently, we don't use google's containers */
/* google's dense_hash_set and dense_hash_map look like a really efficient data structures
 *
 * - std::map and std::set perform better, when containing less than 10 elements
 */
/* using google::dense_hash_map; */
/* using google::dense_hash_set; */
#endif

/* some basic math functions */
inline bool ispoweroftwo(int i)
{
    return (i & (i - 1)) == 0;
}


template <unsigned int n>
struct is_power_of_two
{
    static const bool val = (n%2==0) && (is_power_of_two<(n>>1)>::val);
};

template <>
struct is_power_of_two<2>
{
    static const bool val = true;
};

inline int log2(int n)
{
    if (unlikely(n <= 0))
        return(0);

#ifdef __GNUC__
    return sizeof(int) * __CHAR_BIT__ - 1 - __builtin_clz( n );
#else
    int r = -1;
    while (n)
    {
        r++;
        n >>= 1;
    }
    return r;
#endif
}

inline int nextpoweroftwo(int n)
{
    n = n - 1;
    const uint bitspace = sizeof(int) * 8 / 2;
    for (uint i = 1; i != bitspace; i *= 2)
        n = n | (n >> i);

    return n + 1;
}

using std::size_t;


/** \brief helper function to run all destructors of a pointer container */
template <class T>
inline void delete_all(T begin, T end)
{
    for (;begin!=end;++begin)
        delete *begin;
}

template <class container>
inline void delete_all(container & c)
{
    BOOST_STATIC_ASSERT((boost::is_pointer<typename container::value_type>::value));
    delete_all(c.begin(), c.end());
}

/** \brief base class for a callback function */
template <typename t>
class runnable
{
public:
    virtual ~runnable(void)
    {}

    virtual t run(void) = 0;
};

/** \brief tag for denoting a deleteable class */
class deleteable
{};


template <typename ForwardIterator>
bool is_sorted(ForwardIterator first, ForwardIterator last)
{
    if(first == last) return true;
    for(ForwardIterator previous = first++; first != last; previous = first++)
        if(*first < *previous) return false;
    return true;
}

/*     template <typename ForwardIterator, typename BinaryPredicate> */
/*     bool is_sorted(ForwardIterator first, ForwardIterator last, BinaryPredicate pred = */
/*         std::less<typename ForwardIterator::value_type> ) */
/*     { */
/*         if(first == last) return true; */
/*         for(ForwardIterator previous = first++; first != last; previous = first++) */
/*             if(pred(*first, *previous)) return false; */
/*         return true; */
/*     } */


/** \brief returns the nth element of a specific type in a container of polymorphic pointers */
template <typename container_t, typename target>
target * find_nth_of_type(container_t const & c, uint n)
{
    uint i = 0;
    for (typename container_t::const_iterator it = c.begin(); it != c.end(); ++it)
    {
        target * ret = dynamic_cast<target*>(*it);
        if (ret)
        {
            if (i == n)
                return ret;
            ++i;
        }
    }
    return 0;
}

/** \brief counts how many objects of type target are part of container */
template <typename container, typename target>
uint count_objects_by_type(container const & c)
{
    uint ret = 0;
    for (typename container::const_iterator it = c.begin(); it != c.end(); ++it)
        if (dynamic_cast<target>(*it))
            ++ret;
    return ret;
}

/** \brief count the instances of target */
template <typename container, typename target>
uint count_by_type(container const & c, uint last_index)
{
    int ret = 0;
    for (typename container::const_iterator it = c.begin(); it != c.begin() + last_index; ++it)
        if (dynamic_cast<target>(*it))
            ++ret;
    return uint(ret);
}

template <class T>
struct default_deleter
{
    void operator()(T * t)
    {
        delete t;
    }
};

struct delayed_deleter
{
    template <typename T>
    inline void operator()(T *);
};

struct checked_deleter
{
    template<class T>
    void operator()(T * x) const
    {
        boost::checked_delete(x);
    }
};


template <typename deleter = checked_deleter >
struct intrusive_refcountable:
    public noncopyable,
    public deleter
{
    intrusive_refcountable(void):
        use_count_(0)
    {}

    virtual ~intrusive_refcountable(void)
    {}

    void add_ref(void)
    {
        ++use_count_;
    }

    void release(void)
    {
        if(--use_count_ == 0)
            deleter::operator()(this);
    }

    inline friend void intrusive_ptr_add_ref(intrusive_refcountable * p)
    {
        p->add_ref();
    }

    inline friend void intrusive_ptr_release(intrusive_refcountable * p)
    {
        p->release();
    }

    boost::detail::atomic_count use_count_;
};


template <class t, class compare = std::less<t> >
struct compare_by_instance
{
    bool operator()(const t * lhs, const t * rhs)
    {
        assert(lhs and rhs);
        compare cmp;
        return cmp(*lhs, *rhs);
    }
};


} /* namespace nova */

#endif /* _UTILS_HPP */
