#include <boost/test/unit_test.hpp>

#include <vector>

#include "../source/server/dsp_thread.hpp"

namespace
{

volatile int gint;

struct dummy_runnable
{
    dummy_runnable(void):
        i(0)
    {}

    void operator()(uint dummy)
    {
        ++i;

        for (int j = 0; j != 1000; ++j)
        {
            int l = gint;
            l += 1;
            gint = l;
        }
    }

    int i;
};

dummy_runnable dummy;
}

typedef nova::dsp_queue_interpreter<dummy_runnable> dsp_queue_interpreter;
typedef nova::dsp_thread_queue_item<dummy_runnable> dsp_thread_queue_item;
typedef nova::dsp_thread_queue<dummy_runnable> dsp_thread_queue;

typedef nova::dsp_threads<dummy_runnable> dsp_threads;

BOOST_AUTO_TEST_CASE( dsp_thread_test_1 )
{
    dsp_threads t(1);
}


BOOST_AUTO_TEST_CASE( dsp_thread_test_2 )
{
    dsp_threads t(5);
    t.start_threads();
    t.terminate_threads();
}


BOOST_AUTO_TEST_CASE( dsp_thread_test_3 )
{
    dsp_threads t(2);
    t.start_threads();

    t.run();

    t.terminate_threads();
}

BOOST_AUTO_TEST_CASE( dsp_thread_test_4 )
{
    dsp_threads t(1);
    t.start_threads();

    std::auto_ptr<dsp_thread_queue> q (new dsp_thread_queue());

    dsp_thread_queue_item * item1 = new dsp_thread_queue_item(dummy, dsp_thread_queue_item::successor_list(), 1);
    q->add_queue_item(item1);

    dsp_thread_queue_item::successor_list sl;
    sl.push_back(item1);

    dsp_thread_queue_item * item2 = new dsp_thread_queue_item(dummy, sl, 0);
    q->add_queue_item(item2);
    q->add_initially_runnable(item2);

    t.reset_queue(q);

    t.run();

    t.terminate_threads();

    BOOST_REQUIRE_EQUAL(item1->get_job().i, 1);
    BOOST_REQUIRE_EQUAL(item2->get_job().i, 1);
}

BOOST_AUTO_TEST_CASE( dsp_thread_test_5 )
{
    dsp_threads t(2);
    t.start_threads();

    std::auto_ptr<dsp_thread_queue> q (new dsp_thread_queue());

    dsp_thread_queue_item * item1 = new dsp_thread_queue_item(dummy, dsp_thread_queue_item::successor_list(), 4);
    q->add_queue_item(item1);

    dsp_thread_queue_item::successor_list sl;
    sl.push_back(item1);

    dsp_thread_queue_item * item2 = new dsp_thread_queue_item(dummy, sl, 0);
    q->add_queue_item(item2);
    q->add_initially_runnable(item2);

    dsp_thread_queue_item * item3 = new dsp_thread_queue_item(dummy, sl, 0);
    q->add_queue_item(item3);
    q->add_initially_runnable(item3);

    dsp_thread_queue_item * item4 = new dsp_thread_queue_item(dummy, sl, 0);
    q->add_queue_item(item4);
    q->add_initially_runnable(item4);

    dsp_thread_queue_item * item5 = new dsp_thread_queue_item(dummy, sl, 0);
    q->add_queue_item(item5);
    q->add_initially_runnable(item5);

    t.reset_queue(q);

    t.run();

    t.terminate_threads();

    BOOST_REQUIRE_EQUAL(item1->get_job().i, 1);
    BOOST_REQUIRE_EQUAL(item2->get_job().i, 1);
    BOOST_REQUIRE_EQUAL(item3->get_job().i, 1);
    BOOST_REQUIRE_EQUAL(item4->get_job().i, 1);
    BOOST_REQUIRE_EQUAL(item5->get_job().i, 1);
}


BOOST_AUTO_TEST_CASE( dsp_thread_test_6 )
{
    dsp_threads t(2);
    t.start_threads();

    std::auto_ptr<dsp_thread_queue> q (new dsp_thread_queue());

    std::vector<dsp_thread_queue_item*> items;

    for (int i = 0; i != 20; ++i)
    {
        items.push_back(new dsp_thread_queue_item(dummy, dsp_thread_queue_item::successor_list(), 0));
        q->add_queue_item(items.back());
        q->add_initially_runnable(items.back());
    }

    t.reset_queue(q);

    const int iterations = 10000;

    for (int i = 0; i != iterations; ++i)
    {
        for (int item = 0; item != 20; ++item)
            BOOST_REQUIRE_EQUAL(items[item]->get_job().i, i);
        t.run();
    }

    t.terminate_threads();

    for (int i = 0; i != 20; ++i)
        BOOST_REQUIRE_EQUAL(items[i]->get_job().i, iterations);
}
