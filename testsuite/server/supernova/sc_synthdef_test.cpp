#include <iostream>
#include <boost/test/unit_test.hpp>

#include "sc/sc_synthdef.hpp"

BOOST_AUTO_TEST_CASE(sc_synthdef_test) {
    std::vector<nova::sc_synthdef> defs = nova::read_synthdef_file("testsuite/default.scsyndef");

    std::cout << defs[0].dump();
}
