#include <boost/test/unit_test.hpp>

#include <vector>

#include "../../source/server/sc_osc_handler.hpp"

using namespace nova;

BOOST_AUTO_TEST_CASE(sc_osc_handler_1) {
    const_cast<server_arguments&>(server_arguments::initialize(0, 0)).udp_port = 54321;
    sc_osc_handler handler(server_arguments::instance());
}

BOOST_AUTO_TEST_CASE(sc_osc_handler_2) {
    const_cast<server_arguments&>(server_arguments::initialize(0, 0)).tcp_port = 54321;
    sc_osc_handler handler(server_arguments::instance());
}
