#include <boost/test/unit_test.hpp>

#include "server/node_graph.hpp"

#include "test_synth.hpp"

using namespace nova;
using namespace std;

BOOST_AUTO_TEST_CASE(simple_synth_test_1) {
    rt_pool.init(1024 * 1024);

    node_graph n;

    node_position_constraint to_root = std::make_pair(n.root_group(), insert);

    {
        test_synth* s = new test_synth(1000, 0);
        n.add_node(s, to_root);
        BOOST_REQUIRE_EQUAL(n.synth_count(), 1u);
        BOOST_REQUIRE_EQUAL(n.find_synth(1000), s);
        n.remove_node(s);
        BOOST_REQUIRE_EQUAL(n.synth_count(), 0u);
    }

    BOOST_REQUIRE_EQUAL(n.group_count(), 1u);

    {
        test_synth* s = new test_synth(1000, 0);
        n.add_node(s /* , node_position_constraint() */);
        BOOST_REQUIRE_EQUAL(n.synth_count(), 1u);
        n.remove_node(s);
        BOOST_REQUIRE_EQUAL(n.synth_count(), 0u);
    }

    BOOST_REQUIRE_EQUAL(n.group_count(), 1u);
}

BOOST_AUTO_TEST_CASE(simple_synth_test_2) {
    node_graph n;

    group* g = new group(1);
    n.add_node(g);
    BOOST_REQUIRE_EQUAL(n.find_group(1), g);
    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE(simple_synth_test_3) {
    node_graph n;

    group* g = new group(1);

    n.add_node(g);

    {
        node_position_constraint to_group = std::make_pair(g, insert);

        test_synth* s = new test_synth(1000, 0);

        n.add_node(s, to_group);
        n.remove_node(s);
    }
    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE(simple_synth_test_4) {
    node_graph n;

    group* g = new group(1);

    n.add_node(g);

    test_synth* s = new test_synth(1000, 0);
    {
        node_position_constraint to_group = std::make_pair(g, insert);

        n.add_node(s, to_group);
    }
    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE(simple_synth_test_5) {
    node_graph n;

    test_synth* s = new test_synth(1000, 0);
    test_synth* s2 = new test_synth(1001, 0);

    n.add_node(s);
    n.add_node(s2);

    n.remove_node(s);
    n.remove_node(s2);
}


BOOST_AUTO_TEST_CASE(simple_synth_test_6) {
    node_graph n;

    test_synth* s1 = new test_synth(1000, 0);
    n.add_node(s1);

    test_synth* s2 = new test_synth(1001, 0);
    {
        node_position_constraint to_group = std::make_pair(n.root_group(), insert);

        n.add_node(s2, to_group);
    }

    test_synth* s3 = new test_synth(1002, 0);
    {
        node_position_constraint position = std::make_pair(s1, after);

        n.add_node(s3, position);
    }

    test_synth* s4 = new test_synth(1003, 0);
    {
        node_position_constraint position = std::make_pair(s1, before);

        n.add_node(s4, position);
    }

    // n.dump("test.dot");

    n.remove_node(s1);
    n.remove_node(s2);
    n.remove_node(s3);
    n.remove_node(s4);
}

BOOST_AUTO_TEST_CASE(free_all_test) {
    node_graph n;

    group* g = new group(1);

    n.add_node(g);
    BOOST_REQUIRE_EQUAL(n.group_count(), 2u);

    node_position_constraint to_group = std::make_pair(g, insert);

    test_synth* s = new test_synth(1000, 0);
    n.add_node(s, to_group);

    test_synth* s2 = new test_synth(1001, 0);
    n.add_node(s2, to_group);

    group* g2 = new group(1002);
    n.add_node(g2, to_group);

    BOOST_REQUIRE_EQUAL(n.synth_count(), 2u);
    BOOST_REQUIRE_EQUAL(n.group_count(), 3u);

    BOOST_REQUIRE(n.find_node(s->id()));
    BOOST_REQUIRE(n.find_node(s2->id()));
    BOOST_REQUIRE(n.find_node(g2->id()));

    n.group_free_all(g);

    BOOST_REQUIRE(!n.find_node(s->id()));
    BOOST_REQUIRE(!n.find_node(s2->id()));
    BOOST_REQUIRE(!n.find_node(g2->id()));

    BOOST_REQUIRE_EQUAL(n.synth_count(), 0u);
    BOOST_REQUIRE_EQUAL(n.group_count(), 2u);

    BOOST_REQUIRE(n.find_node(g->id()));
    n.remove_node(g);
    BOOST_REQUIRE(!n.find_node(g->id()));
    BOOST_REQUIRE_EQUAL(n.group_count(), 1u);
}

BOOST_AUTO_TEST_CASE(free_deep_test) {
    node_graph n;

    group* g = new group(1);

    n.add_node(g);
    BOOST_REQUIRE_EQUAL(n.group_count(), 2u);

    node_position_constraint to_group = std::make_pair(g, insert);

    test_synth* s = new test_synth(1000, 0);
    n.add_node(s, to_group);

    test_synth* s2 = new test_synth(1001, 0);
    n.add_node(s2, to_group);

    group* g2 = new group(1002);
    n.add_node(g2, to_group);

    BOOST_REQUIRE_EQUAL(n.synth_count(), 2u);
    BOOST_REQUIRE_EQUAL(n.group_count(), 3u);

    BOOST_REQUIRE(n.find_node(s->id()));
    BOOST_REQUIRE(n.find_node(s2->id()));
    BOOST_REQUIRE(n.find_node(g2->id()));

    n.group_free_deep(g);

    BOOST_REQUIRE(!n.find_node(s->id()));
    BOOST_REQUIRE(!n.find_node(s2->id()));
    BOOST_REQUIRE(n.find_node(g2->id()));

    BOOST_REQUIRE_EQUAL(n.synth_count(), 0u);
    BOOST_REQUIRE_EQUAL(n.group_count(), 3u);

    BOOST_REQUIRE(n.find_node(g2->id()));
    BOOST_REQUIRE(n.find_node(g->id()));
    n.remove_node(g);
    BOOST_REQUIRE(!n.find_node(g->id()));
    BOOST_REQUIRE(!n.find_node(g2->id()));
    BOOST_REQUIRE_EQUAL(n.group_count(), 1u);
}


BOOST_AUTO_TEST_CASE(queue_construction_test_1) {
    node_graph n;

    test_synth* s = new test_synth(1000, 0);
    test_synth* s2 = new test_synth(1001, 0);

    n.add_node(s);
    n.add_node(s2);

    unique_ptr<node_graph::dsp_thread_queue> q = n.generate_dsp_queue();

    n.remove_node(s);
    n.remove_node(s2);
}

BOOST_AUTO_TEST_CASE(queue_construction_test_2) {
    node_graph n;

    group* g = new group(1);

    n.add_node(g);
    unique_ptr<node_graph::dsp_thread_queue> q = n.generate_dsp_queue();
    BOOST_REQUIRE_EQUAL(q->total_node_count(), 0u);

    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE(queue_construction_test_3) {
    node_graph n;

    test_synth* s = new test_synth(1000, 0);
    n.add_node(s);

    unique_ptr<node_graph::dsp_thread_queue> q1 = n.generate_dsp_queue();
    BOOST_REQUIRE_EQUAL(q1->total_node_count(), 1u);

    test_synth* s2 = new test_synth(3, 0);
    n.add_node(s2);

    unique_ptr<node_graph::dsp_thread_queue> q2 = n.generate_dsp_queue();
    BOOST_REQUIRE_EQUAL(q2->total_node_count(), 1u);

    n.remove_node(s);
    n.remove_node(s2);
}


BOOST_AUTO_TEST_CASE(pgroup_test_1) {
    node_graph n;

    parallel_group* g = new parallel_group(1);
    n.add_node(g);
    BOOST_REQUIRE_EQUAL(n.find_group(1), g);

    unique_ptr<node_graph::dsp_thread_queue> q = n.generate_dsp_queue();
    BOOST_REQUIRE_EQUAL(q->total_node_count(), 0u);
    n.remove_node(g);
}


BOOST_AUTO_TEST_CASE(pgroup_test_2) {
    node_graph n;

    parallel_group* g = new parallel_group(1);

    n.add_node(g);

    {
        node_position_constraint to_group = std::make_pair(g, insert);

        test_synth* s = new test_synth(2, 0);
        n.add_node(s, to_group);
        n.remove_node(s);
    }
    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE(pgroup_test_3) {
    node_graph n;

    parallel_group* g = new parallel_group(1);

    n.add_node(g);

    test_synth* s = new test_synth(2, 0);
    {
        node_position_constraint to_group = std::make_pair(g, insert);

        n.add_node(s, to_group);
    }
    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE(pgroup_test_4) {
    node_graph n;

    parallel_group* g = new parallel_group(1);

    n.add_node(g);

    {
        node_position_constraint to_group = std::make_pair(g, insert);

        test_synth* s = new test_synth(2, 0);
        n.add_node(s, to_group);

        unique_ptr<node_graph::dsp_thread_queue> q = n.generate_dsp_queue();
        BOOST_REQUIRE_EQUAL(q->total_node_count(), 1u);

        n.remove_node(s);
    }
    n.remove_node(g);
}


BOOST_AUTO_TEST_CASE(pgroup_test_5) {
    node_graph n;

    parallel_group* g = new parallel_group(1);

    n.add_node(g);

    {
        node_position_constraint to_group = std::make_pair(g, insert);

        test_synth* s1 = new test_synth(2, 0);
        test_synth* s2 = new test_synth(3, 0);
        n.add_node(s1, to_group);
        n.add_node(s2, to_group);

        unique_ptr<node_graph::dsp_thread_queue> q = n.generate_dsp_queue();

        BOOST_REQUIRE_EQUAL(q->total_node_count(), 2u);

        n.remove_node(s1);
        n.remove_node(s2);
    }
    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE(pgroup_test_6) {
    node_graph n;

    parallel_group* g = new parallel_group(1);

    n.add_node(g);

    {
        node_position_constraint to_group = std::make_pair(g, insert);

        test_synth* s1 = new test_synth(2, 0);
        test_synth* s2 = new test_synth(3, 0);
        test_synth* s3 = new test_synth(4, 0);
        n.add_node(s1, to_group);
        n.add_node(s2, to_group);
        n.add_node(s3, to_group);

        unique_ptr<node_graph::dsp_thread_queue> q = n.generate_dsp_queue();
        BOOST_REQUIRE_EQUAL(q->total_node_count(), 3u);
    }
    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE(pgroup_test_7) {
    node_graph n;

    parallel_group* g = new parallel_group(1);

    n.add_node(g);

    {
        node_position_constraint to_group = std::make_pair(g, insert);

        group* g1 = new group(2);
        group* g2 = new group(3);
        group* g3 = new group(4);
        n.add_node(g1, to_group);
        n.add_node(g2, to_group);
        n.add_node(g3, to_group);

        node_position_constraint to_g1 = std::make_pair(g1, insert);
        node_position_constraint to_g2 = std::make_pair(g2, insert);
        test_synth* s1 = new test_synth(1000, 0);
        test_synth* s2 = new test_synth(1001, 0);
        n.add_node(s1, to_g1);
        n.add_node(s2, to_g2);

        unique_ptr<node_graph::dsp_thread_queue> q = n.generate_dsp_queue();
        BOOST_REQUIRE_EQUAL(q->total_node_count(), 2u);
    }
    n.remove_node(g);
}

BOOST_AUTO_TEST_CASE(noid_test) {
    rt_pool.init(1024 * 1024);

    node_graph n;

    node_position_constraint to_root = std::make_pair(n.root_group(), insert);

    {
        test_synth* s = new test_synth(1000, 0);
        n.add_node(s, to_root);
        BOOST_REQUIRE(s->id() == 1000);
        n.synth_reassign_id(1000);
        BOOST_REQUIRE(s->id() != 1000);
        n.remove_node(s);
    }

    {
        test_synth* s = new test_synth(1000, 0);
        n.add_node(s /* , node_position_constraint() */);
        BOOST_REQUIRE(s->id() == 1000);
        n.synth_reassign_id(1000);
        BOOST_REQUIRE(s->id() != 1000);
        n.remove_node(s);
    }
}
