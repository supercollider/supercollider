//  dsp thread queue
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

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include <cstdio>

#include <boost/lockfree/stack.hpp>
#include <boost/mpl/if.hpp>

#include "nova-tt/pause.hpp"

#include "utilities/branch_hints.hpp"
#include "utilities/utils.hpp"

namespace nova {

template <typename runnable, typename Alloc> class dsp_queue_interpreter;

/*
concept runnable
{
    runnable(const & runnable);

    operator()(uint threadindex);
};
*/

/** item of a dsp thread queue
 *
 * \tparam Alloc allocator for successor list
 *
 */
template <typename runnable, typename Alloc = std::allocator<void*>> class dsp_thread_queue_item : private Alloc {
    typedef nova::dsp_queue_interpreter<runnable, Alloc> dsp_queue_interpreter;

    typedef typename Alloc::template rebind<dsp_thread_queue_item>::other new_allocator;

public:
    typedef std::uint_fast16_t activation_limit_t;

    struct successor_list {
        struct data_t {
            uint32_t count;
            uint32_t size;
            dsp_thread_queue_item* content[0];
        };

        typedef typename Alloc::template rebind<data_t>::other array_allocator;

        /* create instance */
        explicit successor_list(uint32_t size = 0) {
            data = array_allocator().allocate(2 * sizeof(uint32_t) + size * sizeof(dsp_thread_queue_item*));
            data->count = 1;
            data->size = size;
        }

        successor_list(successor_list const& rhs): data(rhs.data) { data->count++; }

        successor_list& operator=(successor_list const& rhs) {
            if (--data->count == 0)
                array_allocator().deallocate(data, 2 * sizeof(uint32_t) + data->size * sizeof(dsp_thread_queue_item*));

            data = rhs.data;
            data->count++;
            return *this;
        }

        std::size_t size(void) const { return data->size; }

        bool empty(void) const { return size() == 0; }

        dsp_thread_queue_item*& operator[](std::size_t index) {
            assert(index < size());
            return data->content[index];
        }

        dsp_thread_queue_item* const& operator[](std::size_t index) const {
            assert(index < size());
            return data->content[index];
        }

        ~successor_list(void) {
            if (--data->count == 0)
                array_allocator().deallocate(data, 2 * sizeof(uint32_t) + data->size * sizeof(dsp_thread_queue_item*));
        }

        data_t* data;
    };

    dsp_thread_queue_item(runnable const& job, successor_list const& successors, activation_limit_t activation_limit):
        activation_count(0),
        job(job),
        successors(successors),
        activation_limit(activation_limit) {}

    dsp_thread_queue_item* run(dsp_queue_interpreter& interpreter, std::uint8_t thread_index) {
        assert(activation_count == 0);

        job(thread_index);

        dsp_thread_queue_item* next = update_dependencies(interpreter);
        reset_activation_count();
        return next;
    }

    /** called from the run method or once, when dsp queue is initialized */
    void reset_activation_count(void) {
        assert(activation_count == 0);
        activation_count.store(activation_limit, std::memory_order_release);
    }

    runnable const& get_job(void) const { return job; }

    runnable& get_job(void) { return job; }

#ifdef DEBUG_DSP_THREADS
    void dump_item(void) {
        using namespace std;
        printf("\titem %p\n", this);
        printf("\tactivation limit %d\n", int(activation_limit));

        if (!successors.empty()) {
            printf("\tsuccessors:\n");
            for (size_t i = 0; i != successors.size(); ++i) {
                printf("\t\t%p\n", successors[i]);
            }
        }
        printf("\n");
    }
#endif

private:
    /** \brief update all successors and possibly mark them as runnable */
    dsp_thread_queue_item* update_dependencies(dsp_queue_interpreter& interpreter) {
        dsp_thread_queue_item* next_item_to_run;
        std::size_t i = 0;
        for (;;) {
            if (i == successors.size())
                return nullptr;

            next_item_to_run = successors[i++]->decrement_activation_count();
            if (next_item_to_run)
                break; // no need to update the next item to run
        }

        // push remaining items to scheduler queue
        while (i != successors.size()) {
            dsp_thread_queue_item* next = successors[i++]->decrement_activation_count();
            if (next)
                interpreter.mark_as_runnable(next);
        }

        return next_item_to_run;
    }

    /** \brief decrement activation count and return this, if it drops to zero
     */
    inline dsp_thread_queue_item* decrement_activation_count() {
        activation_limit_t current = activation_count--;
        assert(current > 0);

        if (current == 1)
            return this;
        else
            return nullptr;
    }

    std::atomic<activation_limit_t> activation_count; /**< current activation count */

    runnable job;
    const successor_list successors; /**< list of successing nodes */
    const activation_limit_t activation_limit; /**< number of precedessors */
};

template <typename T, typename Alloc> class raw_vector : Alloc {
public:
    explicit raw_vector(size_t elements, Alloc const& alloc = Alloc()): Alloc(alloc), capacity_(elements) {
        data = elements ? Alloc::allocate(capacity_ * sizeof(T)) : nullptr;
    }

    template <class... Args> T* emplace_back(Args&&... args) {
        assert(size() != capacity_);

        T* element = data + size_;
        Alloc::construct(element, std::forward<Args>(args)...);
        size_ += 1;
        return element;
    }

    T& operator[](std::size_t index) {
        assert(index < size_);
        return data[index];
    }

    T* begin() { return data; }
    T* end() { return data + size_; }
    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }

    ~raw_vector() {
        for (std::size_t i = 0; i != size_; ++i)
            Alloc::destroy(data + i);
        if (data)
            Alloc::deallocate(data, capacity_ * sizeof(Alloc));
    }

private:
    T* data = nullptr;
    const size_t capacity_ = 0;
    size_t size_ = 0;
};

template <typename runnable, typename Alloc = std::allocator<void*>> class dsp_thread_queue {
    typedef std::uint_fast16_t node_count_t;

    typedef nova::dsp_thread_queue_item<runnable, Alloc> dsp_thread_queue_item;
    typedef std::vector<dsp_thread_queue_item*, typename Alloc::template rebind<dsp_thread_queue_item*>::other>
        item_vector_t;

    typedef typename Alloc::template rebind<dsp_thread_queue_item>::other item_allocator;

public:
#ifdef DEBUG_DSP_THREADS
    void dump_queue(void) {
        using namespace std;

        printf("queue %p\n items:\n", this);
        for (std::size_t i = 0; i != total_node_count; ++i)
            queue_items[i].dump_item();
        printf("\ninitial items:\n", this);
        for (dsp_thread_queue_item* item : initially_runnable_items)
            item->dump_item();

        printf("\n");
        std::cout << std::endl;
    }
#endif

    /** preallocate node_count nodes */
    dsp_thread_queue(std::size_t node_count, bool has_parallelism = true):
        has_parallelism_(has_parallelism),
        items(node_count) {
        initially_runnable_items.reserve(node_count);
    }

    ~dsp_thread_queue(void) = default;

    void add_initially_runnable(dsp_thread_queue_item* item) { initially_runnable_items.push_back(item); }

    /** return initialized queue item */
    dsp_thread_queue_item* allocate_queue_item(runnable const& job,
                                               typename dsp_thread_queue_item::successor_list const& successors,
                                               typename dsp_thread_queue_item::activation_limit_t activation_limit) {
        return items.emplace_back(job, successors, activation_limit);
    }

    void reset_activation_counts(void) {
        for (dsp_thread_queue_item& item : items)
            item.reset_activation_count();
    }

    bool empty() const { return items.empty(); }
    node_count_t total_node_count(void) const { return node_count_t(items.size()); }
    bool has_parallelism(void) const { return has_parallelism_; }

private:
    item_vector_t initially_runnable_items; /* nodes without precedessor */
    const bool has_parallelism_;

    friend class dsp_queue_interpreter<runnable, Alloc>;

    raw_vector<dsp_thread_queue_item, item_allocator> items;
};

template <typename runnable, typename Alloc = std::allocator<void*>> class dsp_queue_interpreter {
protected:
    typedef nova::dsp_thread_queue<runnable, Alloc> dsp_thread_queue;
    typedef nova::dsp_thread_queue_item<runnable, Alloc> dsp_thread_queue_item;
    typedef typename dsp_thread_queue_item::successor_list successor_list;
    typedef std::size_t size_t;

public:
    typedef boost::uint_fast8_t thread_count_t;
    typedef boost::uint_fast16_t node_count_t;

    typedef std::unique_ptr<dsp_thread_queue> dsp_thread_queue_ptr;

    dsp_queue_interpreter(thread_count_t tc, bool yield_if_busy = false): yield_if_busy(yield_if_busy) {
        if (!runnable_items.is_lock_free())
            std::cout << "Warning: scheduler queue is not lockfree!" << std::endl;

        calibrate_backoff(10);
        set_thread_count(tc);
    }

    /** prepares queue and queue interpreter for dsp tick
     *
     *  \return true, if dsp queue is valid
     *          false, if no dsp queue is available or queue is empty
     */
    bool init_tick(void) {
        if (unlikely(!queue || queue->empty()))
            return false;

        /* reset node count */
        assert(node_count == 0);
        assert(runnable_items.empty());
        node_count.store(queue->total_node_count(), std::memory_order_release);

        for (auto* item : queue->initially_runnable_items)
            mark_as_runnable(item);

        return true;
    }

    dsp_thread_queue_ptr release_queue(void) {
        dsp_thread_queue_ptr ret(queue.release());
        return ret;
    }

    dsp_thread_queue_ptr reset_queue(dsp_thread_queue_ptr&& new_queue) {
        dsp_thread_queue_ptr ret(std::move(queue));

        queue = std::move(new_queue);
        if (queue.get() == nullptr)
            return ret;

        queue->reset_activation_counts();

#ifdef DEBUG_DSP_THREADS
        queue->dump_queue();
#endif

        if (queue->has_parallelism()) {
            thread_count_t thread_number = std::min(
                thread_count_t(std::min(total_node_count(), node_count_t(std::numeric_limits<thread_count_t>::max()))),
                thread_count);

            used_helper_threads = thread_number - 1; /* this thread is not waked up */
        } else
            used_helper_threads = 0;
        return ret;
    }

    node_count_t total_node_count(void) const { return queue->total_node_count(); }

    void set_thread_count(thread_count_t i) {
        assert(i < std::numeric_limits<thread_count_t>::max());
        i = std::max(thread_count_t(1u), i);
        thread_count = i;
    }

    thread_count_t get_thread_count(void) const { return thread_count; }

    thread_count_t get_used_helper_threads(void) const { return used_helper_threads; }

    void tick(thread_count_t thread_index) {
        if (yield_if_busy)
            run_item<true>(thread_index);
        else
            run_item<false>(thread_index);
    }

private:
    static const int max_backup_loops = 16384;

    struct backoff {
        backoff(int min, int max): min(min), max(max), loops(min) {}

        void run() {
            for (int i = 0; i != loops; ++i)
                nova::detail::pause();

            loops = std::min(loops * 2, max);
        }

        void reset() { loops = min; }

        const int min, max;
        int loops;
    };

    struct yield_backoff {
        yield_backoff(int dummy_min, int dummy_max) {}

        void run() { std::this_thread::yield(); }

        void reset() {}
    };

    template <bool YieldBackoff> struct select_backoff {
        typedef typename boost::mpl::if_c<YieldBackoff, yield_backoff, backoff>::type type;
    };

    void calibrate_backoff(int timeout_in_seconds) {
        using namespace std;
        using namespace std::chrono;

        const int backoff_iterations = 100;

        vector<nanoseconds> measured_values;
        generate_n(back_inserter(measured_values), 16, [] {
            backoff b(max_backup_loops, max_backup_loops);
            auto start = high_resolution_clock::now();

            for (int i = 0; i != backoff_iterations; ++i)
                b.run();

            auto end = high_resolution_clock::now();
            auto diff = duration_cast<nanoseconds>(end - start);

            return diff;
        });

        std::sort(measured_values.begin(), measured_values.end());
        auto median = measured_values[measured_values.size() / 2];

        watchdog_iterations = (seconds(timeout_in_seconds) / median) * backoff_iterations;
    }

    template <bool YieldBackoff> void run_item(thread_count_t index) {
        // note: in future we can avoid the watchdog on osx and linux, as they provide proper
        //       deadline scheduling policies

        typedef typename select_backoff<YieldBackoff>::type backoff_t;

        backoff_t b(8, max_backup_loops);
        int poll_counts = 0;

        for (;;) {
            if (!node_count.load(std::memory_order_acquire))
                return;

            /* we still have some nodes to process */
            int state = run_next_item(index);
            switch (state) {
            case no_remaining_items:
                return;
            case fifo_empty:
                b.run();
                ++poll_counts;
                break;

            case remaining_items:
                b.reset();
                poll_counts = 0;
            }

            if (YieldBackoff)
                continue;

            if (poll_counts == watchdog_iterations) {
                if (index == 0) {
                    std::printf(
                        "nova::dsp_queue_interpreter::run_item: possible lookup detected in main audio thread\n");
                    abort();
                } else {
                    std::printf(
                        "nova::dsp_queue_interpreter::run_item: possible lookup detected in dsp helper thread\n");
                    return;
                }
            }
        }
    }

public:
    void tick_master(void) {
        if (yield_if_busy)
            run_item_master<true>();
        else
            run_item_master<false>();
    }

private:
    template <bool YieldBackoff> void run_item_master(void) {
        run_item<YieldBackoff>(0);
        wait_for_end<YieldBackoff>();
        assert(runnable_items.empty());
    }

    template <bool YieldBackoff> void wait_for_end(void) {
        typedef typename select_backoff<YieldBackoff>::type backoff_t;

        backoff_t b(8, max_backup_loops);
        const int iterations = watchdog_iterations * 2;
        int count = 0;
        while (node_count.load(std::memory_order_acquire) != 0) {
            b.run();
            ++count;
            if (!YieldBackoff && (count == iterations)) {
                std::printf("nova::dsp_queue_interpreter::wait_for_end: possible lookup detected\n");
            }
        } // busy-wait for helper threads to finish
    }

    HOT int run_next_item(thread_count_t index) {
        dsp_thread_queue_item* item;
        bool success = runnable_items.pop(item);

        if (!success)
            return fifo_empty;

        node_count_t consumed = 0;

        do {
            item = item->run(*this, index);
            consumed += 1;
        } while (item != nullptr);

        node_count_t remaining = node_count.fetch_sub(consumed, std::memory_order_release);

        assert(remaining >= consumed);

        if (remaining == consumed)
            return no_remaining_items;
        else
            return remaining_items;
    }

    void mark_as_runnable(dsp_thread_queue_item* item) { runnable_items.push(item); }

    friend class nova::dsp_thread_queue_item<runnable, Alloc>;

private:
    enum { no_remaining_items, fifo_empty, remaining_items };

    dsp_thread_queue_ptr queue;

    thread_count_t thread_count; /* number of dsp threads to be used by this queue */
    thread_count_t used_helper_threads; /* number of helper threads, which are actually used */

    boost::lockfree::stack<dsp_thread_queue_item*, boost::lockfree::capacity<32768>> runnable_items;
    std::atomic<node_count_t> node_count = { 0 }; /* number of nodes, that need to be processed during this tick */
    int watchdog_iterations;
    bool yield_if_busy;
};

} /* namespace nova */
