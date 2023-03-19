#include <boost/test/unit_test.hpp>

#include <string>

#include "utilities/static_pool.hpp"

using namespace nova;
using namespace std;

namespace {

struct foo {
    foo(void) {}
    int data[4];
};

}

BOOST_AUTO_TEST_CASE(static_pool_test) {
    static_pool<16 * 1024> pool;

    foo* f1 = static_cast<foo*>(pool.malloc(sizeof(foo)));
    ::new (f1) foo();
    f1->~foo();
    pool.free(f1);
}

BOOST_AUTO_TEST_CASE(static_pool_test_locked) {
    static_pool<16 * 1024> pool(true);

    foo* f1 = static_cast<foo*>(pool.malloc(sizeof(foo)));
    ::new (f1) foo();
    f1->~foo();
    pool.free(f1);
}
