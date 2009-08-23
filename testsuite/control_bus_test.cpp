#include <iostream>
#include <boost/test/unit_test.hpp>

#include "../source/server/control_bus_manager.hpp"

using namespace nova;

BOOST_AUTO_TEST_CASE( control_bus_test )
{
    control_bus bus(1.234);
}


BOOST_AUTO_TEST_CASE( control_bus_manager_test )
{
    control_bus_manager bus_manager(1024);
}
