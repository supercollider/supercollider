#include <boost/test/unit_test.hpp>

#include <vector>
#include <set>

#include "server/memory_pool.hpp"

namespace nova {
simple_pool<false> rt_pool;
} /* namespace nova */

using namespace nova;

BOOST_AUTO_TEST_CASE(init_pool) { rt_pool.init(1024 * 1024); }


BOOST_AUTO_TEST_CASE(vector_test) {
    rt_pool.init(1024 * 1024);

    int size = 1024;
    std::vector<int, nova::rt_pool_allocator<int>> vec;
    for (int i = 0; i != size; ++i)
        vec.push_back(-i);
}

BOOST_AUTO_TEST_CASE(set_test) {
    int size = 1024;
    std::set<int, std::less<int>, nova::rt_pool_allocator<int>> vec;
    for (int i = 0; i != size; ++i)
        vec.insert(-i);
}
