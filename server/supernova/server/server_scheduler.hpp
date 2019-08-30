//  server scheduler
//  Copyright (C) 2008, 2010 Tim Blechmann
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

#pragma once

#include <mutex>

#include "dsp_thread_queue/dsp_thread.hpp"
#include "group.hpp"
#include "memory_pool.hpp"
#include "utilities/branch_hints.hpp"
#include "utilities/callback_system.hpp"
#include "utilities/static_pooled_class.hpp"

namespace nova {

/** audio-thread synchronization callback
 *
 *  callback for non-rt to rt thread synchronization. since it is using
 *  a locked internal memory pool, instances should not be allocated
 *  from the real-time thread
 *
 * */
struct audio_sync_callback
    : public static_pooled_class<audio_sync_callback, 1 << 20 /* 1mb pool of realtime memory */, true, 16> {
    virtual ~audio_sync_callback() = default;

    virtual void run(void) = 0;
};

/** scheduler class of the nova server
 *
 *  - provides a callback system to place callbacks in the scheduler
 *  - manages dsp threads, which themselves manage the dsp queue interpreter
 *
 *  scheduler_hook: functor to be called when after callbacks have been executed
 *                  and before the threads are executed
 *
 *  thread_init_functor: helper thread initialization functor
 *
 * */
template <class thread_init_functor = nop_thread_init> class scheduler {
    typedef nova::dsp_thread_pool<queue_node, thread_init_functor, rt_pool_allocator<void*>> dsp_thread_pool;
    typedef typename dsp_thread_pool::dsp_thread_queue_ptr dsp_thread_queue_ptr;
    typedef typename dsp_thread_pool::thread_count_t thread_count_t;

    struct reset_queue_cb : public audio_sync_callback {
    public:
        reset_queue_cb(scheduler* sched, dsp_thread_queue_ptr& qptr): sched(sched), qptr(qptr) {}

        void run(void) override {
            sched->reset_queue_sync(qptr);
            /** todo: later free the queue in a helper thread */
        }

    private:
        scheduler* sched;
        dsp_thread_queue_ptr qptr;
    };

protected:
    /* called from the driver callback */
    void reset_queue_sync(dsp_thread_queue_ptr&& qptr) { threads.reset_queue(std::move(qptr)); }

public:
    /* start thread_count - 1 scheduler threads */
    scheduler(thread_count_t thread_count = 1, bool realtime = false):
        threads(thread_count, !realtime, thread_init_functor(realtime)) {}

    void start_dsp_threads(void) { threads.start_threads(); }

    void terminate() {
        cbs.run_callbacks(); // audio backend must be closed by now
        threads.terminate_threads();
    }

    void add_sync_callback(audio_sync_callback* cb) {
        /* we need to guard, because it can be called from the main (system) thread and the network receiver thread */
        std::lock_guard<std::mutex> lock(sync_mutex);
        cbs.add_callback(cb);
    }

    /* called from the audio driver */
    void run_callbacks() { cbs.run_callbacks(); }

    void compute_audio() { threads.run(); }

    /* schedule to set a new queue */
    void reset_queue(dsp_thread_queue_ptr& qptr) { add_sync_callback(new reset_queue_cb(this, qptr)); }

private:
    callback_system<audio_sync_callback, false> cbs;
    dsp_thread_pool threads;
    std::mutex sync_mutex;
};

} /* namespace nova */
