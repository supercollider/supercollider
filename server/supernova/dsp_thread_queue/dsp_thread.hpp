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

#include <cstdint>
#include <iostream>
#include <thread>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/sync/semaphore.hpp>

#include "dsp_thread_queue.hpp"
#include "../utilities/malloc_aligned.hpp"
#include "nova-tt/mlock.hpp"

namespace nova {

using std::uint16_t;

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
    public thread_init_functor
{
    typedef nova::dsp_queue_interpreter<runnable, Alloc> dsp_queue_interpreter;

public:
    dsp_thread(dsp_queue_interpreter & interpreter, uint16_t index, size_t stack_size,
               thread_init_functor const & thread_init = thread_init_functor()):
        thread_init_functor(thread_init), interpreter(interpreter), stop(false), index(index), stack_ (nullptr)
    {
        if (stack_size) {
                stack_ = malloc_aligned<char>(stack_size);
            if (stack_ == nullptr)
                throw std::bad_alloc();
            // touch stack to avoid page faults
            for (size_t i = 0; i != stack_size; ++i)
                stack_[i] = 0;
            mlock(stack_, stack_size);
        }
    }

    dsp_thread(dsp_thread const &) = delete;
    dsp_thread& operator=(dsp_thread const &) = delete;

    ~dsp_thread(void)
    {
        if (stack_)
            free_aligned(stack_);
    }

    /** thread function
     * */
    void run(void)
    {
        thread_init_functor::operator()(index);

        for (;;) {
            cycle_sem.wait();
            if (unlikely(stop.load(std::memory_order_acquire)))
                return;

            interpreter.tick(index);
        }
    }

    static void * run_static(void* arg)
    {
        dsp_thread * self = static_cast<dsp_thread*>(arg);
        self->run();
        return nullptr;
    }

    void terminate(void)
    {
        stop.store(true, std::memory_order_release);
        wake_thread();
    }

    void wake_thread(void)
    {
        cycle_sem.post();
    }


    char * stack(void)
    {
        return stack_;
    }

private:
    boost::sync::semaphore cycle_sem;
    dsp_queue_interpreter & interpreter;
    std::atomic<bool> stop;
    uint16_t index;
    char * stack_;
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

    typedef std::unique_ptr<dsp_thread_queue<runnable, Alloc> > dsp_thread_queue_ptr;

    dsp_threads(thread_count_t count, bool yield_if_busy = false,
                thread_init_functor const & init_functor = thread_init_functor()):
        interpreter(std::min(count, (thread_count_t)std::thread::hardware_concurrency()), yield_if_busy)
    {
        set_dsp_thread_count(interpreter.get_thread_count(), init_functor);
    }

    void run(void)
    {
        bool run_tick = interpreter.init_tick();
        if (likely(run_tick)) {
            wake_threads();
            interpreter.tick_master();
        }
    }

    /** reset queue
     *
     *  don't call, if threads are currently accessing the queue
     * */
    dsp_thread_queue_ptr reset_queue(dsp_thread_queue_ptr && new_queue)
    {
        dsp_thread_queue_ptr ret = interpreter.reset_queue(std::move(new_queue));
        return std::move(ret);
    }

    dsp_thread_queue_ptr release_queue(void)
    {
        return interpreter.release_queue();
    }

private:
#if _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
    // we can set the stack
    static const int stack_size = 524288;
    struct thread_group
    {
        void push_back(pthread_t t)
        {
            thread_group_.push_back(t);
        }

        void join_all(void)
        {
            for(pthread_t & thread : thread_group_) {
                void * ret;
                int err = pthread_join(thread, &ret);
                if (err)
                    printf("Error when joining helper thread\n");
            }
        }

        std::vector<pthread_t> thread_group_;
    };

    void start_threads_impl(void)
    {
        for(dsp_thread & thread : threads) {
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            int err = pthread_attr_setstack(&attr, thread.stack(), stack_size);
            if (err)
                throw std::logic_error("Cannot set stack of DSP helper thread");

            pthread_t thread_id;
            err = pthread_create(&thread_id, &attr, dsp_thread::run_static, &thread);
            if (err)
                throw std::runtime_error("Cannot create DSP helper thread");
            thread_group_.push_back(thread_id);
            pthread_attr_destroy(&attr);
        }
    }
#else
    struct thread_group
    {
        void join_all(void)
        {
            for (std::thread & thread : threads)
                thread.join();
        }

        std::vector<std::thread> threads;
    };

    static const int stack_size = 0;

    void start_threads_impl(void)
    {
        for(dsp_thread & thread : threads)
            thread_group_.threads.push_back(std::move(std::thread(std::bind(&dsp_thread::run, std::ref(thread)))));
    }

#endif

    void set_dsp_thread_count(thread_count_t count, thread_init_functor const & init_functor)
    {
        for (thread_count_t i = 1; i != count; ++i)
            threads.push_back(new dsp_thread(interpreter, i, stack_size, init_functor));
        assert(threads.size() == std::size_t(count-1));
    }

public:
    /** thread handling */
    /* @{ */
    void start_threads(void)
    {
        start_threads_impl();
    }

    void terminate_threads(void)
    {
        for (dsp_thread & thread : threads)
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
    thread_group thread_group_;
};

} /* namespace nova */

#endif /* DSP_THREAD_QUEUE_DSP_THREAD_HPP */
