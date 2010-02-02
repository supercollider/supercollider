#include <boost/test/unit_test.hpp>

#include <vector>

#include "../source/server/sc_osc_handler.hpp"

using namespace nova;

BOOST_AUTO_TEST_CASE( sc_osc_handler_1 )
{
    sc_osc_handler handler(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 43456);
}
