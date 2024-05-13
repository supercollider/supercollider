#include <boost/test/unit_test.hpp>

#include "server/synth_factory.hpp"

#include "test_synth.hpp"

using namespace nova;
using namespace std;

namespace {
struct test_synth_definition : public synth_definition {
    test_synth_definition(): synth_definition(symbol("foo")) {}

    abstract_synth* create_instance(int node_id) { return new test_synth(node_id, this); }
};
}

BOOST_AUTO_TEST_CASE(synth_factory_test_1) {
    rt_pool.init(1 << 20);

    synth_factory sf;

    sf.register_definition(new test_synth_definition);

    unique_ptr<abstract_synth> s(sf.create_instance("foo", 1));

    BOOST_REQUIRE(s.get() != 0);
}
