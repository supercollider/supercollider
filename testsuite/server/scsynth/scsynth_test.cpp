#include <iostream>
#include <boost/test/unit_test.hpp>

#include "SC_WorldOptions.h"
#include "SC_World.h"
#include "SC_Prototypes.h"

BOOST_AUTO_TEST_CASE(create_and_destroy_world) {
    WorldOptions options;
    options.mRealTime = false;
    World* world = World_New(&options);
    World_Cleanup(world, true);
}
