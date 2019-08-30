//  dsp thread
//  Copyright (C) 2007-2015 Tim Blechmann
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

#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include <boost/sync/semaphore.hpp>

#include "dsp_thread_queue.hpp"
#include "malloc_aligned.hpp"
#include "nova-tt/mlock.hpp"

namespace nova {

using std::uint16_t;

struct nop_thread_init {
    nop_thread_init(void) {}

    template <typename Arg> nop_thread_init(Arg const&) {}

    void operator()(int thread_index) {}
};


#if _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
#    define SUPERNOVA_USE_PTHREAD
#endif

/** dsp helper thread
 *
 *  the dsp helper threads are running with a high real-time priority and are
 *  pinned to a specific cpu
 */
template <typename runnable, typename thread_init_functor = nop_thread_init, typename Alloc = std::allocator<void*>>
class dsp_thread : public thread_init_functor {
    typedef nova::dsp_queue_interpreter<runnable, Alloc> dsp_queue_interpreter;

public:
    dsp_thread(dsp_queue_interpreter& interpreter, uint16_t index,
               thread_init_functor const& thread_init = thread_init_functor()):
        thread_init_functor(thread_init),
        interpreter(interpreter),
        index(index) {
#ifdef SUPERNOVA_USE_PTHREAD
        if (stack_size) {
            stack_ = malloc_aligned<char>(stack_size);
            if (stack_ == nullptr)
                throw std::bad_alloc();
            // touch stack to avoid page faults
            for (size_t i = 0; i != stack_size; ++i)
                stack_[i] = 0;
            mlock(stack_, stack_size);
        }
#endif
    }

    dsp_thread(dsp_thread const&) = delete;
    dsp_thread& operator=(dsp_thread const&) = delete;

    ~dsp_thread(void) {
#ifdef SUPERNOVA_USE_PTHREAD
        if (stack_)
            free_aligned(stack_);
#endif
    }

    void start() {
        stop = false;

#ifdef SUPERNOVA_USE_PTHREAD
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        int err = pthread_attr_setstack(&attr, stack_, stack_size);
        if (err)
            throw std::logic_error("Cannot set stack of DSP helper thread");

        err = pthread_create(&thread_id, &attr, run_static, this);
        if (err)
            throw std::runtime_error("Cannot create DSP helper thread");
        pthread_attr_destroy(&attr);
#else
        thread = std::thread([this] { this->run(); });
#endif
    }

    void join() {
        stop.store(true, std::memory_order_relaxed);
        wake_thread();

#ifdef SUPERNOVA_USE_PTHREAD
        void* ret;
        int err = pthread_join(thread_id, &ret);
        if (err)
            printf("Error when joining helper thread\n");
#else
        thread.join();
#endif
    }

    void wake_thread(void) { cycle_sem.post(); }

private:
    /** thread function
     * */
    void run(void) {
        thread_init_functor::operator()(index);

        for (;;) {
            cycle_sem.wait();
            if (unlikely(stop.load(std::memory_order_relaxed)))
                return;

            interpreter.tick(index);
        }
    }

    static void* run_static(void* arg) {
        dsp_thread* self = static_cast<dsp_thread*>(arg);
        self->run();
        return nullptr;
    }

private:
    boost::sync::semaphore cycle_sem;
    dsp_queue_interpreter& interpreter;
    std::atomic<bool> stop = { false };
    uint16_t index;

#ifdef SUPERNOVA_USE_PTHREAD
    pthread_t thread_id;

    static const size_t stack_size = 524288;
    char* stack_ = nullptr;

#else
    std::thread thread;
#endif
};

/** \brief container for all dsp threads
 *
 *  - no care is taken, that dsp_thread_pool::run is executed on a valid instance
 *
 * */
template <typename runnable, typename thread_init_functor = nop_thread_init, typename Alloc = std::allocator<void*>>
class dsp_thread_pool {
    typedef nova::dsp_queue_interpreter<runnable, Alloc> dsp_queue_interpreter;
    typedef nova::dsp_thread<runnable, thread_init_functor, Alloc> dsp_thread;

public:
    typedef typename dsp_queue_interpreter::node_count_t node_count_t;
    typedef typename dsp_queue_interpreter::thread_count_t thread_count_t;

    typedef std::unique_ptr<dsp_thread_queue<runnable, Alloc>> dsp_thread_queue_ptr;

    dsp_thread_pool(thread_count_t count, bool yield_if_busy = false,
                    thread_init_functor const& init_functor = thread_init_functor()):
        interpreter(std::min(count, (thread_count_t)std::thread::hardware_concurrency()), yield_if_busy) {
        set_dsp_thread_count(interpreter.get_thread_count(), init_functor);
    }

    void run(void) {
        const bool run_tick = interpreter.init_tick();
        if (likely(run_tick)) {
            wake_threads();
            interpreter.tick_master();
        }
    }

    /** reset queue
     *
     *  don't call, if threads are currently accessing the queue
     * */
    dsp_thread_queue_ptr reset_queue(dsp_thread_queue_ptr&& new_queue) {
        dsp_thread_queue_ptr ret = interpreter.reset_queue(std::move(new_queue));
        return std::move(ret);
    }

    dsp_thread_queue_ptr release_queue(void) { return interpreter.release_queue(); }

public:
    /** thread handling */
    /* @{ */
    void start_threads(void) {
        for (auto& thread : threads)
            thread->start();
    }

    void terminate_threads(void) {
        for (auto& thread : threads)
            thread->join();
    }
    /* @} */

private:
    void set_dsp_thread_count(thread_count_t count, thread_init_functor const& init_functor) {
        for (thread_count_t i = 1; i != count; ++i)
            threads.emplace_back(new dsp_thread(interpreter, i, init_functor));
    }

    /** wake dsp threads */
    void wake_threads(void) {
        for (thread_count_t i = 0; i != interpreter.get_used_helper_threads(); ++i)
            threads[i]->wake_thread();
    }

    dsp_queue_interpreter interpreter;

    std::vector<std::unique_ptr<dsp_thread>> threads;
};

} /* namespace nova */
