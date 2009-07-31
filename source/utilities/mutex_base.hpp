//  $Id$
//
//  mutex framework base classes
//  Copyright (C) 2007 Tim Blechmann
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


#ifndef _MUTEX_BASE_HPP
#define _MUTEX_BASE_HPP

#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"

namespace nova
{
template <typename T>
struct ref_ptr_wrapper
{
public:
    ref_ptr_wrapper(T * t):
        ptr_(t)
    {}

    ref_ptr_wrapper(T & t):
        ptr_(&t)
    {}

    T * get(void) const
    {
        return ptr_;
    }

    operator T *(void) const
    {
        return ptr_;
    }

private:
    T* ptr_;
};

template <class mutex_type>
class lockable_base:
    public boost::noncopyable
{
public:
    void lock(void) const
    {
        mutex.lock();
    }

    bool try_lock(void) const
    {
        return mutex.try_lock();
    }

    void unlock(void) const
    {
        mutex.unlock();
    }

protected:
    /** \brief hide default constructor */
    lockable_base(void)
    {
    }

    mutable mutex_type mutex;

    template <class mutex_type_,
        void (mutex_type_::*lock)(void),
        void (mutex_type_::*unlock)(void) >
    friend class scoped_lock_base;
};

/** \brief template of a scoped lock class
 *
 * */
template <class mutex_type,
    void (mutex_type::*lock)(void),
    void (mutex_type::*unlock)(void) >
class scoped_lock_base:
    boost::noncopyable
{
public:
    scoped_lock_base (ref_ptr_wrapper<mutex_type> const & mutex) :
        mutex_ (mutex.get())
    {
        (mutex_->*lock)();
    }

    scoped_lock_base (ref_ptr_wrapper<const lockable_base<mutex_type> > const & l) :
        mutex_ (&(l.get()->mutex))
    {
        (mutex_->*lock)();
    }

    template <class T>
    scoped_lock_base (boost::shared_ptr<T> const & l) :
        mutex_ (&(l.get()->mutex))
    {
        (mutex_->*lock)();
    }


    ~scoped_lock_base (void)
    {
        (mutex_->*unlock)();
    }

protected:
    mutex_type * mutex_;
};

} /* namespace nova */

#endif /* _MUTEX_BASE_HPP */
