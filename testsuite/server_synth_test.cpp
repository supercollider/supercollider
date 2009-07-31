#include <boost/test/unit_test.hpp>

#include "../source/server/synth.hpp"

using namespace nova;
using namespace std;

namespace
{
struct test_synth_prototype:
    public synth_prototype
{
    test_synth_prototype(void):
        synth_prototype("synthname")
    {
        register_slot("foo", 0);
    }

    synth * create_instance(int node_id)
    {
        return new synth(node_id, this);
    }
};
}

BOOST_AUTO_TEST_CASE( synth_set_test_1 )
{
    test_synth_prototype * tsp = new test_synth_prototype;
    auto_ptr<synth> s(tsp->create_instance(1));

    s->set("foo", 1.2);
    s->set("foo", 1.2);
}

namespace
{
typedef nova::timer_system<time_tag>::timer_callback timer_callback;

struct test_timer_callback:
    public timer_callback
{
    test_timer_callback(int & i, time_tag const & tt):
        timer_callback(tt), i(i)
    {}

    void run(void)
    {
        ++i;
    }

    int & i;
};

dsp_context context(44100, 64, 0);

}


BOOST_AUTO_TEST_CASE( synth_run_test_1 )
{
    test_synth_prototype * tsp = new test_synth_prototype;
    auto_ptr<synth> s(tsp->create_instance(1));

    int i = 0;
    s->set_timer(new test_timer_callback(i, time_tag::from_samples(32, 44100)));
    s->set_timer(new test_timer_callback(i, time_tag::from_samples(64, 44100)));

    s->run(context);

    BOOST_REQUIRE_EQUAL(i, 2);
}

BOOST_AUTO_TEST_CASE( synth_run_test_2 )
{
    test_synth_prototype * tsp = new test_synth_prototype;
    auto_ptr<synth> s(tsp->create_instance(1));

    int i = 0;

    s->run(context);

    s->set_timer(new test_timer_callback(i, time_tag::from_samples(32, 44100)));
    s->set_timer(new test_timer_callback(i, time_tag::from_samples(64, 44100)));
    s->set_timer(new test_timer_callback(i, time_tag::from_samples(96, 44100)));

    s->run(context);

    BOOST_REQUIRE_EQUAL(i, 3);
}
