#include <boost/test/unit_test.hpp>

#include "sc/sc_synth_prototype.hpp"
#include "server/synth_factory.hpp"

using namespace nova;
using namespace std;

BOOST_AUTO_TEST_CASE(sc_synth_factory) {
    synth_factory factory;

    const char* home_ptr = getenv("HOME");
    BOOST_REQUIRE(home_ptr != 0);

    path home(home_ptr);

    register_synthdefs(factory, sc_read_synthdefs_dir(home / "share/SuperCollider/synthdefs"));
}
