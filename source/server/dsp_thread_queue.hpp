//  dsp thread queue
//  Copyright (C) 2007, 2008 Tim Blechmann
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

#ifndef SERVER_DSP_THREAD_QUEUE_HPP
#define SERVER_DSP_THREAD_QUEUE_HPP

#include <vector>
#include <memory>
#include <algorithm>

#include <boost/lockfree/atomic_int.hpp>
#include <boost/cstdint.hpp>
#include <boost/thread.hpp>

#include <boost/lockfree/fifo.hpp>

#include "utilities/semaphore.hpp"
#include "utilities/branch_hints.hpp"

namespace nova
{

template <typename runnable, typename Alloc>
class dsp_queue_interpreter;

template <typename runnable, typename Alloc>
class dsp_threads;

/*
concept runnable
{
    runnable(const & runnable);

    operator()(uint threadindex);
};

*/

/** item of a dsp thread queue
 *
 * \tparam Alloc allocator for successor list and operator new/delete
 *
 * \todo operator new doesn't support stateful allocators
 */
template <typename runnable,
          typename Alloc = std::allocator<void*> >
class dsp_thread_queue_item:
    private Alloc
{
    typedef boost::uint_fast16_t activation_limit_t;
    typedef nova::dsp_queue_interpreter<runnable, Alloc> dsp_queue_interpreter;

    typedef typename Alloc::template rebind<dsp_thread_queue_item>::other new_allocator;

public:
    void* operator new(std::size_t size)
    {
        return new_allocator().allocate(size);
    }

    inline void operator delete(void * p)
    {
        new_allocator().deallocate((dsp_thread_queue_item*)p, sizeof(dsp_thread_queue_item));
    }

    typedef std::vector<dsp_thread_queue_item*, Alloc> successor_list;

    dsp_thread_queue_item(runnable const & job, successor_list const & successors,
                          activation_limit_t activation_limit):
        activation_count(0), job(job), successors(successors), activation_limit(activation_limit)
    {}

    void run(dsp_queue_interpreter & interpreter, boost::uint8_t thread_index)
    {
        assert(activation_count == 0);

        job(thread_index);

        update_dependencies(interpreter);
        reset_activation_count();
    }

    /** called from the run method or once, when dsp queue is initialized */
    void reset_activation_count(void)
    {
        assert(activation_count == 0);
        activation_count += activation_limit;
    }

    runnable const & get_job(void) const
    {
        return job;
    }

private:
    /** \brief update all successors and possibly mark them as runnable */
    void update_dependencies(dsp_queue_interpreter & interpreter)
    {
        for (std::size_t i = 0; i != successors.size(); ++i)
            successors[i]->dec_ref_count(interpreter);
    }

    /** \brief decrement reference count and possibly mark as runnable */
    inline void dec_ref_count(dsp_queue_interpreter & interpreter)
    {
        if (activation_count-- == 1)
            interpreter.mark_as_runnable(this);
    }

    boost::lockfree::atomic_int<activation_limit_t> activation_count; /**< current activation count */

    runnable job;
    successor_list successors;                                 /**< list of successing nodes */
    const activation_limit_t activation_limit;                 /**< number of precedessors */
};

template <typename runnable, typename Alloc = std::allocator<void*> >
class dsp_thread_queue
{
    typedef boost::uint_fast16_t node_count_t;

    typedef nova::dsp_thread_queue_item<runnable, Alloc> dsp_thread_queue_item;

public:
    dsp_thread_queue(void):
        total_node_count(0)
    {}

    ~dsp_thread_queue(void)
    {
        for (std::size_t i = 0; i != queue_items.size(); ++i)
            delete queue_items[i];
    }

    void add_initially_runnable(dsp_thread_queue_item * item)
    {
        initially_runnable_items.push_back(item);
    }

    /** takes ownership */
    void add_queue_item(dsp_thread_queue_item * item)
    {
        queue_items.push_back(item);
        ++total_node_count;

        assert (total_node_count == queue_items.size());
    }

    void reset_activation_counts(void)
    {
        assert(total_node_count == queue_items.size());

        for (node_count_t i = 0; i != total_node_count; ++i)
            queue_items[i]->reset_activation_count();
    }

    node_count_t get_total_node_count(void) const
    {
        return total_node_count;
    }

private:
    node_count_t total_node_count;      /* total number of nodes */

    typename dsp_thread_queue_item::successor_list initially_runnable_items; /* nodes without precedessor */
    std::vector<dsp_thread_queue_item*> queue_items;                         /* all nodes */

    friend class dsp_queue_interpreter<runnable, Alloc>;
};

template <typename runnable,
          typename Alloc = std::allocator<void*> >
class dsp_queue_interpreter
{
protected:
    typedef nova::dsp_thread_queue<runnable, Alloc> dsp_thread_queue;
    typedef nova::dsp_thread_queue_item<runnable, Alloc> dsp_thread_queue_item;
    typedef typename dsp_thread_queue_item::successor_list successor_list;
    typedef std::size_t size_t;

public:
    typedef boost::uint_fast8_t thread_count_t;
    typedef boost::uint_fast16_t node_count_t;

    typedef std::auto_ptr<dsp_thread_queue> dsp_thread_queue_ptr;

    dsp_queue_interpreter(thread_count_t tc):
        fifo(1024), node_count(0)
    {
        set_thread_count(tc);
    }

    void tick(thread_count_t thread_index)
    {
        for(;;)
        {
            bool done = run_item(thread_index);
            if (unlikely(done))
                return;
        }
    }

    void tick_master(void)
    {
        for(;;)
        {
            bool done = run_item_master();
            if (unlikely(done))
            {
                assert(end_sem.value() == 0);
                assert(sem.value() == 0);
                return;
            }
        }
    }

    /** prepares queue and queue interpreter for dsp tick
     *
     *  \return true, if dsp queue is valid
     *          false, if no dsp queue is available or queue is empty
     */
    bool init_tick(void)
    {
        if (unlikely((queue.get() == NULL) or                /* no queue */
                     (queue->get_total_node_count() == 0)    /* no nodes */
                    ))
            return false;

        /* reset node count */
        assert(node_count == 0);
        node_count += queue->get_total_node_count(); /* this is definitely atomic! */

        successor_list const & initially_runnable_items = queue->initially_runnable_items;
        for (size_t i = 0; i != initially_runnable_items.size(); ++i)
            mark_as_runnable(initially_runnable_items[i]);

        qdone = false;
        return true;
    }

    dsp_thread_queue_ptr release_queue(void)
    {
        dsp_thread_queue_ptr ret(queue.release());
        return ret;
    }

    dsp_thread_queue_ptr reset_queue(dsp_thread_queue_ptr & new_queue)
    {
        dsp_thread_queue_ptr ret(queue.release());
        queue = new_queue;
        if (queue.get() == 0)
            return ret;

        queue->reset_activation_counts();

        thread_count_t thread_number =
            std::min(thread_count_t(std::min(total_node_count(),
                                             node_count_t(std::numeric_limits<thread_count_t>::max()))),
                     thread_count);

        used_helper_threads = thread_number - 1; /* this thread is not waked up */
        return ret;
    }

    node_count_t total_node_count(void) const
    {
        return queue->get_total_node_count();
    }

    void set_thread_count(thread_count_t i)
    {
        assert (i < std::numeric_limits<thread_count_t>::max());
        i = std::max(thread_count_t(1u), i);
        thread_count = i;
    }

    thread_count_t get_thread_count(void) const
    {
        return thread_count;
    }

    thread_count_t get_used_helper_threads(void) const
    {
        return used_helper_threads;
    }

private:
    bool run_item(thread_count_t index)
    {
        for (;;)
        {
            if (sem.try_wait())
            {

                bool done = run_next_item(index);

                /* wake master thread on end */
                if (unlikely(done))
                {
                    assert(node_count == 0);
                    qdone = true;
                    end_sem.post(); /* wake master */

                    if (used_helper_threads > 0) /* wake helper threads */
                        for (int i = 1; i != used_helper_threads; ++i)
                            done_sem.post();
                    return true;
                }
                return false;
            }
            if (done_sem.try_wait())
                return true;
        }
    }

    bool run_item_master(void)
    {
        for(;;)
        {
            if (sem.try_wait())
            {
                bool cont = run_next_item(0);

                /* wake other threads on end */
                if (likely(cont))
                {
                    assert(node_count == 0);
                    qdone = true;

                    if (used_helper_threads > 0) /* wake helper threads */
                        for (int i = 0; i != used_helper_threads; ++i)
                            done_sem.post();
                    /* end master thread loop */
                    return true;
                }
            }
            if (end_sem.try_wait())
                return true;
        }
    }


    bool run_next_item(thread_count_t index)
    {
        dsp_thread_queue_item * item;
        bool success = fifo.dequeue(&item);
        assert(success);

        item->run(*this, index);

        node_count_t remaining = node_count--;

        return (remaining == 1);
    }

    friend class nova::dsp_threads<runnable, Alloc>;

    void mark_as_runnable(dsp_thread_queue_item * item)
    {
        fifo.enqueue(item);
        sem.post();
    }

    friend class nova::dsp_thread_queue_item<runnable, Alloc>;

private:
    dsp_thread_queue_ptr queue;
    semaphore sem;               /* semaphore for dsp threads */
    semaphore end_sem;           /* semaphore for main audio thread to wait for */
    semaphore done_sem;          /* semaphore for helper threads to wait for */

    bool qdone;

    thread_count_t thread_count;        /* number of dsp threads to be used by this queue */
    thread_count_t used_helper_threads; /* number of helper threads, which are actually used */

    boost::lockfree::fifo<dsp_thread_queue_item*> fifo;
    boost::lockfree::atomic_int<node_count_t> node_count; /* number of nodes, that need to be processed during this tick */
};

} /* namespace nova */

#endif /* SERVER_DSP_THREAD_QUEUE_HPP */
