#include <boost/test/unit_test.hpp>

#include "../source/server/synth_factory.hpp"

using namespace nova;
using namespace std;

namespace
{
struct test_synth_prototype:
    public synth_prototype
{
    test_synth_prototype():
        synth_prototype("foo")
    {}

    synth * create_instance(int node_id)
    {
        return new synth(node_id, this);
    }
};
}

BOOST_AUTO_TEST_CASE( synth_factory_test_1 )
{
    synth_factory sf;

    sf.register_prototype(new test_synth_prototype);

    auto_ptr<synth> s(sf.create_instance("foo", 1));

    BOOST_REQUIRE(s.get() != 0);
}
