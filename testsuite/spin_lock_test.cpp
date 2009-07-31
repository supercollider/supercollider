#include <boost/test/unit_test.hpp>

#include "../source/utilities/spin_lock.hpp"

#include <boost/thread/thread.hpp>

using namespace nova;
using namespace boost;

namespace
{

int count = 0;

spin_lock sl;

void test_fn(void)
{
    for (int i = 0; i != 10000; ++i)
    {
        spin_lock::scoped_lock lock(sl);
        ++count;
    }
}


}

BOOST_AUTO_TEST_CASE( spinlock_test )
{
    thread_group g;

    for (int i = 0; i != 10; ++i)
        g.create_thread(test_fn);
    g.join_all();

    BOOST_REQUIRE_EQUAL(count, 100000);
}
