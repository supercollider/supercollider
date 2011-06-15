//  dsp thread
//  Copyright (C) 2007, 2008, 2009, 2010 Tim Blechmann
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

#ifndef DSP_THREAD_QUEUE_DSP_THREAD_HPP
#define DSP_THREAD_QUEUE_DSP_THREAD_HPP

#include <iostream>

#include <boost/bind.hpp>
#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/thread/thread.hpp>

#ifndef foreach
#define foreach BOOST_FOREACH
#endif

#include "dsp_thread_queue.hpp"

namespace nova
{

using boost::uint16_t;

struct nop_thread_init
{
    nop_thread_init(void)
    {}

    template <typename Arg>
    nop_thread_init(Arg const &)
    {}

    void operator()(int thread_index)
    {}
};


/** dsp helper thread
 *
 *  the dsp helper threads are running with a high real-time priority and are
 *  pinned to a specific cpu
 */
template <typename runnable,
          typename thread_init_functor = nop_thread_init,
          typename Alloc = std::allocator<void*>
         >
class dsp_thread:
    public boost::noncopyable,
    public thread_init_functor
{
    typedef nova::dsp_queue_interpreter<runnable, Alloc> dsp_queue_interpreter;

public:
    dsp_thread(dsp_queue_interpreter & interpreter, uint16_t index,
               thread_init_functor const & thread_init = thread_init_functor()):
        interpreter(interpreter), stop(false), index(index), thread_init_functor(thread_init)
    {}

    /** thread function
     * */
    void run(void)
    {
        thread_init_functor::operator()(index);

        for (;;)
        {
            cycle_sem.wait();
            if (unlikely(stop.load(boost::memory_order_acquire)))
                return;

            interpreter.tick(index);
        }
    }
    void terminate(void)
    {
        stop.store(true, boost::memory_order_release);
        wake_thread();
    }

    void wake_thread(void)
    {
        cycle_sem.post();
    }

private:
    semaphore cycle_sem;
    dsp_queue_interpreter & interpreter;
    boost::atomic<bool> stop;
    uint16_t index;
};

/** \brief container for all dsp threads
 *
 *  - no care is taken, that dsp_threads::run is executed on a valid instance
 *
 * */
template <typename runnable,
          typename thread_init_functor = nop_thread_init,
          typename Alloc = std::allocator<void*>
         >
class dsp_threads
{
    typedef nova::dsp_queue_interpreter<runnable, Alloc> dsp_queue_interpreter;

    typedef nova::dsp_thread<runnable, thread_init_functor, Alloc> dsp_thread;


public:
    typedef typename dsp_queue_interpreter::node_count_t node_count_t;
    typedef typename dsp_queue_interpreter::thread_count_t thread_count_t;

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    typedef std::unique_ptr<dsp_thread_queue<runnable, Alloc> > dsp_thread_queue_ptr;
#else
    typedef std::auto_ptr<dsp_thread_queue<runnable, Alloc> > dsp_thread_queue_ptr;
#endif

    dsp_threads(thread_count_t count, thread_init_functor const & init_functor = thread_init_functor()):
        interpreter(std::min(count, (thread_count_t)boost::thread::hardware_concurrency()))
    {
        set_dsp_thread_count(interpreter.get_thread_count(), init_functor);
    }

    void run(void)
    {
        bool run_tick = interpreter.init_tick();
        if (likely(run_tick))
        {
            wake_threads();
            interpreter.tick_master();
        }
    }

    /** reset queue
     *
     *  don't call, if threads are currently accessing the queue
     * */
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    dsp_thread_queue_ptr reset_queue(dsp_thread_queue_ptr && new_queue)
    {
        dsp_thread_queue_ptr ret = interpreter.reset_queue(std::move(new_queue));
        return std::move(ret);
    }
#else
    dsp_thread_queue_ptr reset_queue(dsp_thread_queue_ptr & new_queue)
    {
        return interpreter.reset_queue(new_queue);
    }
#endif

    dsp_thread_queue_ptr release_queue(void)
    {
        return interpreter.release_queue();
    }

private:
    void set_dsp_thread_count(thread_count_t count, thread_init_functor const & init_functor)
    {
        for (thread_count_t i = 1; i != count; ++i)
            threads.push_back(new dsp_thread(interpreter, i, init_functor));
        assert(threads.size() == std::size_t(count-1));
    }

public:
    /** thread handling */
    /* @{ */
    void start_threads(void)
    {
        foreach(dsp_thread & thread, threads)
            thread_group_.create_thread(boost::bind(&dsp_thread::run, boost::ref(thread)));
    }

    void terminate_threads(void)
    {
        foreach(dsp_thread & thread, threads)
            thread.terminate();

        thread_group_.join_all();
    }
    /* @} */

private:
    /** wake dsp threads */
    void wake_threads(void)
    {
        for (thread_count_t i = 0; i != interpreter.get_used_helper_threads(); ++i)
            threads[i].wake_thread();
    }

    dsp_queue_interpreter interpreter;

    boost::ptr_vector<dsp_thread> threads; /* container of dsp threads */

    boost::thread_group thread_group_;
};

} /* namespace nova */

#endif /* DSP_THREAD_QUEUE_DSP_THREAD_HPP */

