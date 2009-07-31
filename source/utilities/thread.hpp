//  $Id$
//
//  thread class
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


#ifndef _THREAD_HPP
#define _THREAD_HPP

#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/barrier.hpp>

#include <cassert>
#include "utils.hpp"

#include <iostream>

#if (_POSIX_TIMERS - 0) >= 200112L
#include <time.h>
#elif defined(WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <time.h>
#else
#error System not supported
#endif /* _POSIX_TIMERS */

#if (_POSIX_PRIORITY_SCHEDULING - 0) >=  200112L || (_POSIX_MEMLOCK - 0) >=  200112L
#include <sched.h>
#endif

#include "semaphore.hpp"

namespace nova
{
typedef pthread_t thread_t;


inline void MicroSleep(unsigned long microsec)
{
#if _POSIX_C_SOURCE >= 199309L

    if (0 && microsec < 1000000)
    {
        struct timespec timeout, remain;
        timeout.tv_sec = 0;
        timeout.tv_nsec = microsec*1000;
        nanosleep (&timeout, &remain);
    }
    else
    {
        struct timeval timeout;
        timeout.tv_sec = microsec / 1000000;
        timeout.tv_usec = microsec % 1000000;
        select(0,0,0,0,&timeout);
    }

#elif defined(WIN32)
    Sleep(microsec/1000);
#error System not supported
#endif
}

inline void yield(void)
{
    boost::thread::yield();
}

/** \brief Condition variable, wrapper for boost condition variables */
class condition:
    public boost::noncopyable
{
public:
    condition()
    {
    }

    void notify()
    {
        m_condition.notify_one();
    }

    void notify_all()
    {
        m_condition.notify_all();
    }

    void wait()
    {
        boost::mutex::scoped_lock lock(m_mutex);
        m_condition.wait(lock);
    }


    void timed_wait(uint secs)
    {
        boost::xtime xxx;
        xxx.sec = secs;

        boost::mutex::scoped_lock lock(m_mutex);
        m_condition.timed_wait(lock, xxx);
    }


private:
    boost::condition m_condition;
    boost::mutex m_mutex;
};

/** \brief Abstract Thread base class */
class Thread:
    public boost::noncopyable
{
public:
    Thread():
        stacksize(0)
    {
    }

    virtual ~Thread()
    {
    }

    /** \brief detach Thread */
    void Detach()
    {
        pthread_detach(thread_id);
    }

    /** \brief get Thread id */
    thread_t GetThreadId() const
    {
        return thread_id;
    }

    void terminate(void)
    {
        pthread_cancel(thread_id);
    }

    void join(void)
    {
        pthread_join(thread_id, NULL);
    }

protected:

    /** \brief virtual Thread function to overload */
    virtual void Run() = 0;

public:
    /** \brief start thread ... waits until the thread has been started */
    void Start()
    {
        pthread_attr_t attr;
        pthread_attr_init (&attr);
        if (stacksize)
            pthread_attr_setstacksize(&attr,stacksize);

        int status = pthread_create (&thread_id, &attr, thread_starter, this);
        assert(status == 0);
        pthread_attr_destroy(&attr);
        sem.wait();
    }

private:
    /** \brief static wrapper for pthread_create */
    static void* thread_starter(void * arg)
    {
        Thread* thisThread = (Thread*)arg;
        thisThread->sem.post();
        thisThread->Run();
        return 0;
    }

    semaphore sem;
    thread_t thread_id; /**< \brief thread id */
    long stacksize;
};


/** \brief Thread, using a Condition  */
/** \todo find a better name :-/ */
class SignalableThread:
    public Thread
{
public:
    SignalableThread():
        sem(1), running_(true)
    {}

    /** \brief stop thread on destruction */
    ~SignalableThread()
    {
        if (running())
            Stop();
    }

    /** \brief Signal, i.e. wake up the thread */
    void Signal(void)
    {
        sem.post();
    }

protected:
    /** \brief this can be used in the virtual Run() function to suspend this thread
     *  until it's signaled
     *  */
    void Wait(void)
    {
        sem.wait();
    }

    void Stop()
    {
        running_ = false;
        Signal();
        join();
    }

    virtual void Run() = 0;

    bool running(void)
    {
        return running_;
    }

private:
    semaphore sem;
    volatile bool running_;
};

/** \brief get the current Thread ID
 *  \return id of current Thread
 *
 *  at least on linux, it seems to be very efficient:
 *  only one mov instruction with nptl
 *  */
inline thread_t GetCurrentThread(void)
{
    return pthread_self();
}

/** \brief set thread priority of the calling thread
 *  \param p
 *  - if P is positive, get minimal priority + p
 *  - else get maximal priority - p
 *  */
/* @{ */
bool SetRTPriority(int p);
bool SetPriority(int p);
/* @} */

} /* namespace nova */

#endif /* _THREAD_HPP */
