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
#include <boost/ptr_container/ptr_vector.hpp>

#include <lockfree/fifo.hpp>

#include "utilities/semaphore.hpp"
#include "utilities/branch_hints.hpp"

namespace nova
{

template <typename runnable>
class dsp_queue_interpreter;

template <typename runnable>
class dsp_threads;

/*
concept runnable
{
    runnable(const & runnable);

    operator()(uint threadindex);
};

*/

/** item of a dsp thread queue */
template <typename runnable>
class dsp_thread_queue_item
{
    typedef boost::uint_fast16_t activation_limit_t;
    typedef nova::dsp_queue_interpreter<runnable> dsp_queue_interpreter;

public:
    typedef std::vector<dsp_thread_queue_item*> successor_list;

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

    lockfree::atomic_int<activation_limit_t> activation_count; /**< current activation count */

    runnable job;
    successor_list successors;                                 /**< list of successing nodes */
    const activation_limit_t activation_limit;                 /**< number of precedessors */
};

template <typename runnable>
class dsp_thread_queue
{
    typedef boost::uint_fast16_t node_count_t;

    typedef nova::dsp_thread_queue_item<runnable> dsp_thread_queue_item;

public:
    dsp_thread_queue(void):
        total_node_count(0)
    {}

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
            queue_items[i].reset_activation_count();
    }

    node_count_t get_total_node_count(void) const
    {
        return total_node_count;
    }

private:
    node_count_t total_node_count;      /* total number of nodes */

    std::vector<dsp_thread_queue_item*> initially_runnable_items; /* nodes without precedessor */
    boost::ptr_vector<dsp_thread_queue_item> queue_items;         /* all nodes */

    friend class dsp_queue_interpreter<runnable>;
};

template <typename runnable>
class dsp_queue_interpreter
{
protected:
    typedef nova::dsp_thread_queue<runnable> dsp_thread_queue;
    typedef nova::dsp_thread_queue_item<runnable> dsp_thread_queue_item;
    typedef std::size_t size_t;

public:
    typedef boost::uint_fast8_t thread_count_t;
    typedef boost::uint_fast16_t node_count_t;

    typedef std::auto_ptr<dsp_thread_queue> dsp_thread_queue_ptr;

    dsp_queue_interpreter(thread_count_t tc):
        queue(new dsp_thread_queue()), node_count(0)
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

    bool init_tick(void)
    {
        /* reset node count */
        assert(node_count == 0);
        node_count += queue->get_total_node_count(); /* this is definitely atomic! */

        if (queue->get_total_node_count() == 0)
            return false;

        std::vector<dsp_thread_queue_item*> const & initially_runnable_items = queue->initially_runnable_items;
        for (size_t i = 0; i != initially_runnable_items.size(); ++i)
            mark_as_runnable(initially_runnable_items[i]);

        queue->reset_activation_counts();

        qdone = false;
        return true;
    }

    dsp_thread_queue_ptr release_queue(void)
    {
        dsp_thread_queue_ptr ret(queue.release());
        queue.reset(new dsp_thread_queue());
        return ret;
    }

    dsp_thread_queue_ptr reset_queue(dsp_thread_queue_ptr & new_queue)
    {
        dsp_thread_queue_ptr ret(queue.release());
        queue = new_queue;

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

    friend class nova::dsp_threads<runnable>;

    void mark_as_runnable(dsp_thread_queue_item * item)
    {
        fifo.enqueue(item);
        sem.post();
    }

    friend class nova::dsp_thread_queue_item<runnable>;

private:
    dsp_thread_queue_ptr queue;
    semaphore sem;               /* semaphore for dsp threads */
    semaphore end_sem;           /* semaphore for main audio thread to wait for */
    semaphore done_sem;          /* semaphore for helper threads to wait for */

    bool qdone;

    thread_count_t thread_count;        /* number of dsp threads to be used by this queue */
    thread_count_t used_helper_threads; /* number of helper threads, which are actually used */

    lockfree::fifo<dsp_thread_queue_item*> fifo;
    lockfree::atomic_int<node_count_t> node_count; /* number of nodes, that need to be processed during this tick */
};

} /* namespace nova */

#endif /* SERVER_DSP_THREAD_QUEUE_HPP */
