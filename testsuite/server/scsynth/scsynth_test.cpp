#include "scsynth_test.hpp"

BOOST_AUTO_TEST_CASE(create_and_destroy_world) {
    WorldOptions options;
    options.mRealTime = false;
    World* world = World_New(&options);
    World_Cleanup(world, true);
}
