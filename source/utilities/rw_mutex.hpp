//  read-write mutex class
//  Copyright (C) 2007, 2009 Tim Blechmann
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


#ifndef RW_MUTEX_HPP
#define RW_MUTEX_HPP

#include <cerrno>
#include <cassert>

#include "boost/thread/locks.hpp"

#include "pthread.h"

#include "branch_hints.hpp"
#include "mutex_base.hpp"


namespace nova
{

template <class mutex_type> class rw_lockable_base;

/** \brief template of a scoped lock class
 *
 * */
template <class mutex_type,
    void (mutex_type::*lock)(void),
    void (mutex_type::*unlock)(void) >
class scoped_rw_lock_base:
    public scoped_lock_base<mutex_type, lock, unlock>
{
    typedef scoped_lock_base<mutex_type, lock, unlock> base_class;

public:
    template <class arg_type>
    scoped_rw_lock_base (arg_type & arg):
        base_class (arg)
    {}

    template <class arg_type>
    scoped_rw_lock_base (arg_type const & arg):
        base_class (arg)
    {}
};

template <class mutex_type>
class rw_lockable_base:
    public lockable_base<mutex_type>
{
public:
    void lock_shared(void) const
    {
        lockable_base<mutex_type>::mutex.lock_shared();
    }

    bool try_lock_shared(void) const
    {
        return lockable_base<mutex_type>::mutex.try_lock_shared();
    }

    void unlock_shared(void) const
    {
        lockable_base<mutex_type>::mutex.unlock_shared();
    }

    void lock(void) const
    {
        lockable_base<mutex_type>::mutex.lock();
    }

    bool try_lock(void) const
    {
        return lockable_base<mutex_type>::mutex.try_lock();
    }

    void unlock(void) const
    {
        lockable_base<mutex_type>::mutex.unlock();
    }

    template <class mutex_type_,
        void (mutex_type_::*lock)(void),
        void (mutex_type_::*unlock)(void) >
    friend class scoped_rw_lock_base;


public:
    typedef scoped_rw_lock_base<mutex_type, &mutex_type::lock_shared, &mutex_type::unlock_shared> scoped_read_lock_;
    typedef scoped_rw_lock_base<mutex_type, &mutex_type::lock, &mutex_type::unlock> scoped_write_lock_;
};


class nonrecursive_rw_mutex
{
public:
    nonrecursive_rw_mutex(void)
    {
        int status = pthread_rwlock_init(&rwlock, NULL);
        assert(status == 0);
    }

    ~nonrecursive_rw_mutex(void)
    {
        int status = pthread_rwlock_destroy(&rwlock);
        assert(status == 0);
    }

    void lock(void)
    {
        int status = pthread_rwlock_wrlock(&rwlock);
        assert(status == 0);
    }

    bool try_lock(void)
    {
        int status = pthread_rwlock_trywrlock(&rwlock);

        switch (status)
        {
            case 0:
                return true;

            case EBUSY:
                return false;

            case EDEADLK:
            default:
                assert(false);
        }
    }

    /* glibc seems to be buggy ... don't unlock more often than it has been locked
     *
     * http://sourceware.org/bugzilla/show_bug.cgi?id=4825
     */
    void unlock(void)
    {
        int status = pthread_rwlock_unlock(&rwlock);
        assert(status == 0);
    }

    void lock_shared(void)
    {
        int status = pthread_rwlock_rdlock(&rwlock);
        assert(status == 0);
    }

    bool try_lock_shared(void)
    {
        int status = pthread_rwlock_tryrdlock(&rwlock);

        if (status == 0)
            return true;
        if (status == EBUSY)
            return false;

        assert(false);
    }

    void unlock_shared(void)
    {
        unlock();
    }

protected:
    pthread_rwlock_t rwlock;

public:
    typedef boost::unique_lock<nonrecursive_rw_mutex> unique_lock;
    typedef boost::shared_lock<nonrecursive_rw_mutex> shared_lock;
};

class rw_mutex:
    public nonrecursive_rw_mutex
{
public:
    typedef boost::unique_lock<nonrecursive_rw_mutex> unique_lock;
    typedef boost::shared_lock<nonrecursive_rw_mutex> shared_lock;

    rw_mutex(void):
        writelock_count(0), writer_id(0)
    {}

    ~rw_mutex(void)
    {
        assert(writer_id == 0);
    }

    void lock(void)
    {
        int status = pthread_rwlock_wrlock(&rwlock);

        switch (status)
        {
        case 0:
            writer_id = pthread_self();
            assert(writelock_count == 0);

        case EDEADLK:
            assert(writer_id == pthread_self());
            ++writelock_count;
            return;

        default:
            assert(false);
        }
    }

    bool try_lock(void)
    {
        int status = pthread_rwlock_trywrlock(&rwlock);

        switch (status)
        {
        case 0:
            assert(writer_id == 0);
            assert(writelock_count == 0);
            writer_id = pthread_self();
            ++writelock_count;
            return true;

        case EDEADLK:
            assert(writer_id == pthread_self());

        case EBUSY:
            if (writer_id == pthread_self())
            {
                assert(writelock_count > 0);
                ++writelock_count;
                return true;
            }
            else
            {
                assert(writer_id != pthread_self());
                return false;
            }

        default:
            assert(false);
        }
    }

    void unlock(void)
    {
        assert(writelock_count > 0);
        assert(writer_id);
        if (--writelock_count == 0)
        {
            writer_id = 0;
            nonrecursive_rw_mutex::unlock();
        }
    }

    void lock_shared(void)
    {
        if (unlikely(writer_id == pthread_self()))
            /* we're already owning it as writer */
            return;
        nonrecursive_rw_mutex::lock_shared();
    }

    bool try_lock_shared(void)
    {
        if (unlikely(writer_id == pthread_self()))
            /* we're already owning it as writer */
            return true;

        return nonrecursive_rw_mutex::try_lock_shared();
    }

    void unlock_shared(void)
    {
        if (unlikely(writer_id == pthread_self()))
            /* we're owning it as writer */
            return;

        int status = pthread_rwlock_unlock(&rwlock);
        assert(status == 0);
    }


private:
    unsigned int writelock_count;
    pthread_t writer_id; /* id of the writer thread
                          * set during the write lock
                          */
};


typedef rw_lockable_base<rw_mutex> rw_lockable;
typedef rw_lockable_base<nonrecursive_rw_mutex> nonrecursive_rw_lockable;

typedef rw_lockable::scoped_read_lock_ scoped_read_lock;
typedef rw_lockable::scoped_write_lock_ scoped_write_lock;
typedef nonrecursive_rw_lockable::scoped_read_lock_ nonrecursive_scoped_read_lock;
typedef nonrecursive_rw_lockable::scoped_write_lock_ nonrecursive_scoped_write_lock;

template <class l>
class typed_scoped_read_lock:
    public boost::noncopyable
{
public:
    typed_scoped_read_lock(const l * obj):
        obj_(obj)
    {
        obj_->lock_shared();
    }

    typed_scoped_read_lock(l const & obj):
        obj_(&obj)
    {
        obj_->lock_shared();
    }

    ~typed_scoped_read_lock(void)
    {
        obj_->unlock_shared();
    }

private:
    const l * obj_;
};

template <class l>
class typed_scoped_write_lock:
    public boost::noncopyable
{
public:
    typed_scoped_write_lock(const l * obj):
        obj_(obj)
    {
        obj_->lock();
    }

    typed_scoped_write_lock(l const & obj):
        obj_(&obj)
    {
        obj_->lock();
    }

    ~typed_scoped_write_lock(void)
    {
        obj_->unlock();
    }

private:
    const l * obj_;
};



} /* namespace nova */

#endif /* RW_MUTEX_HPP */
