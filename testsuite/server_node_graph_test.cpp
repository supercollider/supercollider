#include <boost/test/unit_test.hpp>

#include "../source/server/node_graph.hpp"
#include "../source/server/synth.hpp"

using namespace nova;
using namespace std;

BOOST_AUTO_TEST_CASE( simple_synth_test_1 )
{
    rt_pool.init(1024 * 1024);

    node_graph n;

    node_position_constraint to_root = std::make_pair(n.root_group(), insert);

    {
        synth * s = new synth(1000, 0);
        n.add_node(s, to_root);
        n.remove_node(s);
    }

    {
        synth * s = new synth(1000, 0);
        n.add_node(s/* , node_position_constraint() */);
        n.remove_node(s);
    }
}

BOOST_AUTO_TEST_CASE( simple_synth_test_2 )
{
    node_graph n;

    group * g = new group(1);
    n.add_node(g);
    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE( simple_synth_test_3 )
{
    node_graph n;

    group * g = new group(1);

    n.add_node(g);

    {
        node_position_constraint to_group = std::make_pair(g, insert);

        synth * s = new synth(1000, 0);

        n.add_node(s, to_group);
        n.remove_node(s);
    }
    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE( simple_synth_test_4 )
{
    node_graph n;

    group * g = new group(1);

    n.add_node(g);

    synth * s = new synth(1000, 0);
    {
        node_position_constraint to_group = std::make_pair(g, insert);

        n.add_node(s, to_group);
    }
    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE( simple_synth_test_5 )
{
    node_graph n;

    synth * s = new synth(1000, 0);
    synth * s2 = new synth(1001, 0);

    n.add_node(s);
    n.add_node(s2);

    n.remove_node(s);
    n.remove_node(s2);
}


BOOST_AUTO_TEST_CASE( simple_synth_test_6 )
{
    node_graph n;

    synth * s1 = new synth(1000, 0);
    n.add_node(s1);

    synth * s2 = new synth(1001, 0);
    {
        node_position_constraint to_group = std::make_pair(n.root_group(), insert);

        n.add_node(s2, to_group);
    }

    synth * s3 = new synth(1002, 0);
    {
        node_position_constraint position = std::make_pair(s1, after);

        n.add_node(s3, position);
    }

    synth * s4 = new synth(1003, 0);
    {
        node_position_constraint position = std::make_pair(s1, before);

        n.add_node(s4, position);
    }

    //n.dump("test.dot");

    n.remove_node(s1);
    n.remove_node(s2);
    n.remove_node(s3);
    n.remove_node(s4);
}

BOOST_AUTO_TEST_CASE( free_all_test )
{
    node_graph n;

    group * g = new group(1);

    n.add_node(g);

    {
        node_position_constraint to_group = std::make_pair(g, insert);

        synth * s = new synth(1000, 0);
        n.add_node(s, to_group);

        synth * s2 = new synth(1001, 0);
        n.add_node(s2, to_group);
    }
    g->free_children();
    n.remove_node(g);
}


BOOST_AUTO_TEST_CASE( queue_construction_test_1 )
{
    node_graph n;

    synth * s = new synth(1000, 0);
    synth * s2 = new synth(1001, 0);

    n.add_node(s);
    n.add_node(s2);

    auto_ptr<node_graph::dsp_thread_queue> q = n.generate_dsp_queue();

    n.remove_node(s);
    n.remove_node(s2);
}

BOOST_AUTO_TEST_CASE( queue_construction_test_2 )
{
    node_graph n;

    group * g = new group(1);

    n.add_node(g);
    auto_ptr<node_graph::dsp_thread_queue> q = n.generate_dsp_queue();

    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE( queue_construction_test_3 )
{
    node_graph n;

    synth * s = new synth(1000, 0);
    n.add_node(s);

    auto_ptr<node_graph::dsp_thread_queue> q1 = n.generate_dsp_queue();
    BOOST_REQUIRE_EQUAL(q1->get_total_node_count(), 1u);

    synth * s2 = new synth(3, 0);
    n.add_node(s2);

    auto_ptr<node_graph::dsp_thread_queue> q2 = n.generate_dsp_queue();
    BOOST_REQUIRE_EQUAL(q2->get_total_node_count(), 2u);

    n.remove_node(s);
    n.remove_node(s2);
}




BOOST_AUTO_TEST_CASE( pgroup_test_1 )
{
    node_graph n;

    parallel_group * g = new parallel_group(1);
    n.add_node(g);
    auto_ptr<node_graph::dsp_thread_queue> q = n.generate_dsp_queue();
    BOOST_REQUIRE_EQUAL(q->get_total_node_count(), 0u);
    n.remove_node(g);
}


BOOST_AUTO_TEST_CASE( pgroup_test_2 )
{
    node_graph n;

    parallel_group * g = new parallel_group(1);

    n.add_node(g);

    {
        node_position_constraint to_group = std::make_pair(g, insert);

        synth * s = new synth(2, 0);
        n.add_node(s, to_group);
        n.remove_node(s);
    }
    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE( pgroup_test_3 )
{
    node_graph n;

    parallel_group * g = new parallel_group(1);

    n.add_node(g);

    synth * s = new synth(2, 0);
    {
        node_position_constraint to_group = std::make_pair(g, insert);

        n.add_node(s, to_group);
    }
    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE( pgroup_test_4 )
{
    node_graph n;

    parallel_group * g = new parallel_group(1);

    n.add_node(g);

    {
        node_position_constraint to_group = std::make_pair(g, insert);

        synth * s = new synth(2, 0);
        n.add_node(s, to_group);

        auto_ptr<node_graph::dsp_thread_queue> q = n.generate_dsp_queue();
        BOOST_REQUIRE_EQUAL(q->get_total_node_count(), 1u);

        n.remove_node(s);
    }
    n.remove_node(g);
}


BOOST_AUTO_TEST_CASE( pgroup_test_5 )
{
    node_graph n;

    parallel_group * g = new parallel_group(1);

    n.add_node(g);

    {
        node_position_constraint to_group = std::make_pair(g, insert);

        synth * s1 = new synth(2, 0);
        synth * s2 = new synth(3, 0);
        n.add_node(s1, to_group);
        n.add_node(s2, to_group);

        auto_ptr<node_graph::dsp_thread_queue> q = n.generate_dsp_queue();

        BOOST_REQUIRE_EQUAL(q->get_total_node_count(), 2u);

        n.remove_node(s1);
        n.remove_node(s2);
    }
    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE( pgroup_test_6 )
{
    node_graph n;

    parallel_group * g = new parallel_group(1);

    n.add_node(g);

    {
        node_position_constraint to_group = std::make_pair(g, insert);

        synth * s1 = new synth(2, 0);
        synth * s2 = new synth(3, 0);
        synth * s3 = new synth(4, 0);
        n.add_node(s1, to_group);
        n.add_node(s2, to_group);
        n.add_node(s3, to_group);

        auto_ptr<node_graph::dsp_thread_queue> q = n.generate_dsp_queue();
        BOOST_REQUIRE_EQUAL(q->get_total_node_count(), 3u);
    }
    n.remove_node(g);
}
