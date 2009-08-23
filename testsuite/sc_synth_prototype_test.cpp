#include <boost/test/unit_test.hpp>

#include "../source/sc/sc_synth_prototype.hpp"
#include "../source/server/synth_factory.hpp"

using namespace nova;
using namespace std;

BOOST_AUTO_TEST_CASE( sc_synth_factory )
{
    synth_factory factory;

    const char * home_ptr = getenv("HOME");
    BOOST_REQUIRE (home_ptr != 0);

    path home(home_ptr);

    sc_read_synthdefs_dir(factory, home / "share/SuperCollider/synthdefs");
}
