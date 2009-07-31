//  $Id$
//
//  Mutex class
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


#ifndef _MUTEX_HPP
#define _MUTEX_HPP

#include "pthread.h"
#include "mutex_base.hpp"

#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>
#include <boost/pointee.hpp>

#include "boost/functional.hpp"
#include "boost/thread/recursive_mutex.hpp"
#include <cerrno>

namespace nova
{

typedef boost::recursive_mutex Mutex;

typedef lockable_base<Mutex> lockable;

class scoped_lock:
    public scoped_lock_base<Mutex, &Mutex::lock, &Mutex::unlock>
{
    typedef scoped_lock_base<Mutex, &Mutex::lock, &Mutex::unlock> base_class;

public:
    template <class arg_type>
    scoped_lock (arg_type & arg) :
        base_class (arg)
    {
    }

    template <class arg_type>
    scoped_lock (arg_type const & arg) :
        base_class (arg)
    {
    }
};

template <class l>
class typed_scoped_lock:
    public boost::noncopyable
{
public:
    typed_scoped_lock(const l * obj):
        obj_(obj)
    {
        obj_->lock();
    }

    typed_scoped_lock(l const & obj):
        obj_(&obj)
    {
        obj_->lock();
    }

    ~typed_scoped_lock(void)
    {
        obj_->unlock();
    }

private:
    const l * obj_;
};

#ifdef DEBUG
#define DEBUG_SCOPED_LOCK(arg) scoped_lock(arg);
#else
#define DEBUG_SCOPED_LOCK(arg)
#endif

class try_lock_failure:
    public std::exception
{
};

template<class base_lock>
class try_scoped_lock_base:
    public base_lock
{
public:
    template <typename ctorarg>
    try_scoped_lock_base(ctorarg const & arg):
        base_lock(arg)
    {
        if (not base_lock::mutex.try_lock())
            throw try_lock_failure();
    }
};

typedef try_scoped_lock_base<scoped_lock> try_scoped_lock;

/** \brief tries to lock all objects of container */
template <typename container>
bool trylock_all(container const & c)
{
    BOOST_STATIC_ASSERT((boost::is_base_of<lockable, typename container::value_type>::value));

    typename container::const_iterator current = c.begin();

    while(current != c.end())
    {
        if (not current.trylock())
        {
            for_each(c.begin(), current, boost::mem_fun(&lockable::unlock));
            return false;
        }
        ++current;
    }
    return true;
}

template <typename container, typename lockable_base, typename lockable_base_base, bool (lockable_base_base::*try_lock_fn)(void) const,
    void (lockable_base_base::*unlock_fn)(void) const>
bool trylock_all2(container const & c)
{

//    BOOST_STATIC_ASSERT((boost::is_base_of<const lockable,
//                                           typename boost::pointee<typename boost::pointee<typename container::iterator>::type>::type >::value));

    typename container::const_iterator current = c.begin();

    while(current != c.end())
    {
        bool locked = (static_cast<lockable_base*>(current->get())->*try_lock_fn)();

        if (not locked)
        {
            for(typename container::const_iterator it = c.begin(); it != current; ++it)
                (static_cast<lockable_base*>(it->get())->*unlock_fn)();

            return false;
        }
        ++current;
    }
    return true;
}

} /* namespace nova */

#endif /* _MUTEX_HPP */
