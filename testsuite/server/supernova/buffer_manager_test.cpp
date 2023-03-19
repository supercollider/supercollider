#include <iostream>
#include <boost/test/unit_test.hpp>

#include "server/buffer_manager.cpp"


using namespace nova;


BOOST_AUTO_TEST_CASE(buffer_manager_test) {
    buffer_manager bm(1024);
    bm.allocate_buffer(0, 44100, 2);
    bm.zero_buffer(0);
    bm.fill_samples(0, 0, 882000, 0.2);
    bm.free_buffer(0);
}
