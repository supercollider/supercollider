#include <boost/test/unit_test.hpp>

#include <vector>

#include "dsp_thread_queue/dsp_thread.hpp"
#include "server/memory_pool.hpp"

namespace {

volatile int gint;

struct dummy_runnable {
    dummy_runnable(void): i(0) {}

    void operator()(uint dummy) {
        ++i;

        for (int j = 0; j != 1000; ++j) {
            int l = gint;
            l += 1;
            gint = l;
        }
    }

    int i;
};

dummy_runnable dummy;
}

template <typename Alloc> void run_test_1(void) {
    nova::dsp_thread_pool<dummy_runnable, nova::nop_thread_init, Alloc> t(1);
}

BOOST_AUTO_TEST_CASE(dsp_thread_test_1) {
    nova::rt_pool.init(1024 * 1024 * 128);
    run_test_1<std::allocator<void*>>();
    run_test_1<nova::rt_pool_allocator<void*>>();
}

template <typename Alloc> void run_test_2(void) {
    nova::dsp_thread_pool<dummy_runnable, nova::nop_thread_init, Alloc> t(5);
    t.start_threads();
    t.terminate_threads();
}


BOOST_AUTO_TEST_CASE(dsp_thread_test_2) {
    run_test_2<std::allocator<void*>>();
    run_test_2<nova::rt_pool_allocator<void*>>();
}

template <typename Alloc> void run_test_3(void) {
    nova::dsp_thread_pool<dummy_runnable, nova::nop_thread_init, Alloc> t(2);
    t.start_threads();
    t.run();
    t.terminate_threads();
}

BOOST_AUTO_TEST_CASE(dsp_thread_test_3) {
    run_test_3<std::allocator<void*>>();
    run_test_3<nova::rt_pool_allocator<void*>>();
}

template <typename Alloc> void run_test_4(void) {
    typedef typename nova::dsp_thread_queue_item<dummy_runnable, Alloc> dsp_thread_queue_item;
    typedef typename nova::dsp_thread_queue<dummy_runnable, Alloc> dsp_thread_queue;

    typedef typename nova::dsp_thread_pool<dummy_runnable, nova::nop_thread_init, Alloc> dsp_thread_pool;
    typedef std::unique_ptr<dsp_thread_queue> dsp_thread_queue_ptr;

    dsp_thread_pool t(1);
    t.start_threads();

    dsp_thread_queue_ptr q(new dsp_thread_queue(2));

    dsp_thread_queue_item* item1 = q->allocate_queue_item(dummy, typename dsp_thread_queue_item::successor_list(), 1);

    typename dsp_thread_queue_item::successor_list sl(1);
    sl[0] = item1;

    dsp_thread_queue_item* item2 = q->allocate_queue_item(dummy, sl, 0);
    q->add_initially_runnable(item2);

    t.reset_queue(std::move(q));

    t.run();

    t.terminate_threads();

    BOOST_REQUIRE_EQUAL(item1->get_job().i, 1);
    BOOST_REQUIRE_EQUAL(item2->get_job().i, 1);
}


BOOST_AUTO_TEST_CASE(dsp_thread_test_4) {
    run_test_4<std::allocator<void*>>();
    run_test_4<nova::rt_pool_allocator<void*>>();
}

template <typename Alloc> void run_test_5(void) {
    typedef nova::dsp_thread_queue_item<dummy_runnable, Alloc> dsp_thread_queue_item;
    typedef nova::dsp_thread_queue<dummy_runnable, Alloc> dsp_thread_queue;
    typedef nova::dsp_thread_pool<dummy_runnable, nova::nop_thread_init, Alloc> dsp_thread_pool;

    typedef std::unique_ptr<dsp_thread_queue> dsp_thread_queue_ptr;

    dsp_thread_pool t(2);
    t.start_threads();

    dsp_thread_queue_ptr q(new dsp_thread_queue(5));

    dsp_thread_queue_item* item1 = q->allocate_queue_item(dummy, typename dsp_thread_queue_item::successor_list(), 4);

    typename dsp_thread_queue_item::successor_list sl(1);
    sl[0] = item1;

    dsp_thread_queue_item* item2 = q->allocate_queue_item(dummy, sl, 0);
    q->add_initially_runnable(item2);

    dsp_thread_queue_item* item3 = q->allocate_queue_item(dummy, sl, 0);
    q->add_initially_runnable(item3);

    dsp_thread_queue_item* item4 = q->allocate_queue_item(dummy, sl, 0);
    q->add_initially_runnable(item4);

    dsp_thread_queue_item* item5 = q->allocate_queue_item(dummy, sl, 0);
    q->add_initially_runnable(item5);

    t.reset_queue(std::move(q));

    t.run();

    t.terminate_threads();

    BOOST_REQUIRE_EQUAL(item1->get_job().i, 1);
    BOOST_REQUIRE_EQUAL(item2->get_job().i, 1);
    BOOST_REQUIRE_EQUAL(item3->get_job().i, 1);
    BOOST_REQUIRE_EQUAL(item4->get_job().i, 1);
    BOOST_REQUIRE_EQUAL(item5->get_job().i, 1);
}


BOOST_AUTO_TEST_CASE(dsp_thread_test_5) {
    run_test_5<std::allocator<void*>>();
    run_test_5<nova::rt_pool_allocator<void*>>();
}

template <typename Alloc> void run_test_6(void) {
    typedef nova::dsp_thread_queue_item<dummy_runnable, Alloc> dsp_thread_queue_item;
    typedef nova::dsp_thread_queue<dummy_runnable, Alloc> dsp_thread_queue;
    typedef nova::dsp_thread_pool<dummy_runnable, nova::nop_thread_init, Alloc> dsp_thread_pool;

    typedef std::unique_ptr<dsp_thread_queue> dsp_thread_queue_ptr;

    dsp_thread_pool t(2);
    t.start_threads();

    dsp_thread_queue_ptr q(new dsp_thread_queue(20));

    std::vector<dsp_thread_queue_item*> items;

    for (int i = 0; i != 20; ++i) {
        items.push_back(q->allocate_queue_item(dummy, typename dsp_thread_queue_item::successor_list(), 0));
        q->add_initially_runnable(items.back());
    }

    t.reset_queue(std::move(q));

    const int iterations = 10000;

    for (int i = 0; i != iterations; ++i) {
        for (int item = 0; item != 20; ++item)
            BOOST_REQUIRE_EQUAL(items[item]->get_job().i, i);
        t.run();
    }

    t.terminate_threads();

    for (int i = 0; i != 20; ++i)
        BOOST_REQUIRE_EQUAL(items[i]->get_job().i, iterations);
}

BOOST_AUTO_TEST_CASE(dsp_thread_test_6) {
    run_test_6<std::allocator<void*>>();
    run_test_6<nova::rt_pool_allocator<void*>>();
}
