#include <iostream>
#include <boost/test/unit_test.hpp>

#include "../../external_libraries/nova-simd/benchmarks/perf_counter.hpp"

BOOST_AUTO_TEST_CASE(perf_count_test) {
    perf_counter pc;

    pc.start();

    pc.stop();

    pc.dump();
}
