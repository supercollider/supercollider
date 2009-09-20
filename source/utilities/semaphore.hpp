//  semaphore class, posix & boost.thread wrappers,
//  based on API proposed in N2043
//
//  Copyright (C) 2008 Tim Blechmann
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

#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <boost/noncopyable.hpp>

#include <cassert>

#if (_POSIX_SEMAPHORES - 0) >= 200112L

#define POSIX_SEMAPHORE_WRAPPER
#include <semaphore.h>

#else
#define BOOST_SEMAPHORE_EMULATION

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#endif


namespace nova
{
#ifdef POSIX_SEMAPHORE_WRAPPER
class semaphore:
    boost::noncopyable
{
public:
    semaphore(uint i=0)
    {
        sem_init(&sem, 0, i);
    }

    ~semaphore(void)
    {
        sem_destroy(&sem);
    }

    /** \brief signal semaphore */
    void post(void)
    {
        int status = sem_post(&sem);
        assert(status == 0);
    }

    /** \brief wait until this semaphore is signaled */
    void wait(void)
    {
        int status = sem_wait(&sem);
        assert(status == 0);
    }

    /** \brief dummy */
    bool try_wait(void)
    {
        int status = sem_trywait(&sem);
        return status == 0;
    }

    int value(void)
    {
        int ret;
        bool status = sem_getvalue(&sem, &ret);
        assert(status == 0);

        if (ret < 0)
            return 0;
        else
            return ret;
    }

private:
    sem_t sem;
};

#elif defined(BOOST_SEMAPHORE_EMULATION)
#warning boost semaphore emulation
class semaphore:
    boost::noncopyable
{
public:
    semaphore(int i=0):
        m_count(i)
    {}

    /** \brief signal semaphore */
    void post(void)
    {
        boost::mutex::scoped_lock lock(m_mutex);
        ++m_count;
        m_cond.notify_one();
    }

    /** \brief wait until this semaphore is signaled */
    void wait(void)
    {
        boost::mutex::scoped_lock lock(m_mutex);
        while (m_count==0)
            m_cond.wait(lock); /** \todo check! valgrind complains about this */
        --m_count;
    }

    int value(void)
    {
        boost::mutex::scoped_lock lock(m_mutex);
        return m_count;
    }

private:
    unsigned long m_count;
    boost::mutex m_mutex;
    boost::condition m_cond;
};

#else
#error this point should not be reached!
#endif

} // namespace nova

#undef POSIX_SEMAPHORE_WRAPPER

#endif /* SEMAPHORE_HPP */
