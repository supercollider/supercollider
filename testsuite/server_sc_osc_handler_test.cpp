#include <boost/test/unit_test.hpp>

#include <vector>

#include "../source/server/sc_osc_handler.hpp"

using namespace nova;

BOOST_AUTO_TEST_CASE( sc_osc_handler_1 )
{
    sc_osc_handler handler(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 43456);
}

BOOST_AUTO_TEST_CASE( sc_osc_handler_2 )
{
    sc_osc_handler handler(AF_INET, SOCK_STREAM, IPPROTO_TCP, 43457);
}
