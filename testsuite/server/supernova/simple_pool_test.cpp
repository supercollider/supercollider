#include <boost/test/unit_test.hpp>

#include <string>

#include "utilities/simple_pool.hpp"

using namespace nova;
using namespace std;

namespace {

struct foo {
    foo(void) {}
    int data[4];
};

void run_simple_test(bool locked) {
    simple_pool<> pool(16 * 1024, locked);

    foo* f1 = static_cast<foo*>(pool.malloc(sizeof(foo)));
    ::new (f1) foo();
    f1->~foo();
    pool.free(f1);
}

void run_simple_test_2(bool locked) {
    simple_pool<> pool;

    pool.init(16 * 1024, locked);

    foo* f1 = static_cast<foo*>(pool.malloc(sizeof(foo)));
    ::new (f1) foo();
    f1->~foo();
    pool.free(f1);
}

}

BOOST_AUTO_TEST_CASE(simple_pool_tests) {
    run_simple_test(false);
    run_simple_test(true);
    run_simple_test_2(false);
    run_simple_test_2(true);
}
