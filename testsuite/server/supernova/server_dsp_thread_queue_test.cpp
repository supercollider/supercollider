#include <boost/test/unit_test.hpp>

#include "dsp_thread_queue/dsp_thread_queue.hpp"

namespace {

struct dummy_runnable {
    dummy_runnable(void): i(0) {}

    void operator()(unsigned int dummy) { ++i; }

    int i;
};

dummy_runnable dummy;
}

typedef nova::dsp_queue_interpreter<dummy_runnable> dsp_queue_interpreter;
typedef nova::dsp_thread_queue_item<dummy_runnable> dsp_thread_queue_item;
typedef nova::dsp_thread_queue<dummy_runnable> dsp_thread_queue;

BOOST_AUTO_TEST_CASE(dsp_thread_queue_test_1) {
    dsp_queue_interpreter interpreter(1);

    bool runnable = interpreter.init_tick();
    BOOST_REQUIRE(!runnable);
}

BOOST_AUTO_TEST_CASE(dsp_thread_queue_test_2) {
    dsp_queue_interpreter interpreter(1);

    bool runnable = interpreter.init_tick();
    if (runnable)
        interpreter.tick(0);
}

BOOST_AUTO_TEST_CASE(dsp_thread_queue_test_3) {
    dsp_queue_interpreter interpreter(1);

    std::unique_ptr<dsp_thread_queue> q(new dsp_thread_queue(1));

    dsp_thread_queue_item* item = q->allocate_queue_item(dummy, dsp_thread_queue_item::successor_list(), 0);

    q->add_initially_runnable(item);

    interpreter.reset_queue(std::move(q));

    bool runnable = interpreter.init_tick();
    BOOST_REQUIRE(runnable);

    interpreter.tick(0);

    BOOST_REQUIRE_EQUAL(item->get_job().i, 1);
}

BOOST_AUTO_TEST_CASE(dsp_thread_queue_test_4) {
    dsp_queue_interpreter interpreter(1);

    std::unique_ptr<dsp_thread_queue> q(new dsp_thread_queue(2));

    dsp_thread_queue_item* item1 = q->allocate_queue_item(dummy, dsp_thread_queue_item::successor_list(), 1);

    dsp_thread_queue_item::successor_list sl(1);
    sl[0] = item1;

    dsp_thread_queue_item* item2 = q->allocate_queue_item(dummy, sl, 0);
    q->add_initially_runnable(item2);

    interpreter.reset_queue(std::move(q));

    bool runnable = interpreter.init_tick();
    BOOST_REQUIRE(runnable);
    interpreter.tick(0);

    BOOST_REQUIRE_EQUAL(item1->get_job().i, 1);
    BOOST_REQUIRE_EQUAL(item2->get_job().i, 1);
}

BOOST_AUTO_TEST_CASE(dsp_thread_queue_test_5) {
    dsp_queue_interpreter interpreter(1);

    {
        std::unique_ptr<dsp_thread_queue> q(new dsp_thread_queue(2));

        dsp_thread_queue_item* item1 = q->allocate_queue_item(dummy, dsp_thread_queue_item::successor_list(), 1);

        dsp_thread_queue_item::successor_list sl(1);
        sl[0] = item1;

        dsp_thread_queue_item* item2 = q->allocate_queue_item(dummy, sl, 0);

        q->add_initially_runnable(item2);

        interpreter.reset_queue(std::move(q));

        for (int i = 0; i != 2; ++i) {
            bool runnable = interpreter.init_tick();
            BOOST_REQUIRE(runnable);
            interpreter.tick(0);
        }
        BOOST_REQUIRE_EQUAL(item1->get_job().i, 2);
        BOOST_REQUIRE_EQUAL(item2->get_job().i, 2);
    }

    {
        std::unique_ptr<dsp_thread_queue> q(new dsp_thread_queue(2));

        dsp_thread_queue_item* item1 = q->allocate_queue_item(dummy, dsp_thread_queue_item::successor_list(), 1);

        dsp_thread_queue_item::successor_list sl(1);
        sl[0] = item1;

        dsp_thread_queue_item* item2 = q->allocate_queue_item(dummy, sl, 0);

        q->add_initially_runnable(item2);

        interpreter.reset_queue(std::move(q));

        for (int i = 0; i != 2; ++i) {
            bool runnable = interpreter.init_tick();
            BOOST_REQUIRE(runnable);
            interpreter.tick(0);
        }
        BOOST_REQUIRE_EQUAL(item1->get_job().i, 2);
        BOOST_REQUIRE_EQUAL(item2->get_job().i, 2);
    }
}
