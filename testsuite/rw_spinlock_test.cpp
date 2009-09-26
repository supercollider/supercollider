#include <boost/test/unit_test.hpp>

#include <boost/thread.hpp>

#include "../source/utilities/rw_spinlock.hpp"

BOOST_AUTO_TEST_CASE( rw_spinlock_test )
{
    using namespace nova;
    rw_spinlock mut;

    mut.lock_shared();
    mut.lock_shared();
    BOOST_REQUIRE_EQUAL( mut.try_lock(), false );
    BOOST_REQUIRE_EQUAL( mut.try_lock_shared(), true );
    {
        rw_spinlock::shared_lock lock(mut);
    }
    mut.unlock_shared();
    mut.unlock_shared();
    mut.unlock_shared();

    BOOST_REQUIRE_EQUAL( mut.try_lock(), true );
    BOOST_REQUIRE_EQUAL( mut.try_lock_shared(), false );
    mut.unlock();
}

namespace
{

using namespace nova;
rw_spinlock guard;

uint32_t dummy = 0;
uint32_t counter = 0;

const uint iterations = 200000;
const uint thread_count = 4;

void test_fn(void)
{
    for (uint i = 0; i != iterations; ++i)
    {
        guard.lock();
        counter += 1;
        guard.unlock();

        for (int j = 0; j != 100; ++j)
        {
            guard.lock_shared();
            dummy += counter;
            guard.unlock_shared();
        }
    }
}

}

BOOST_AUTO_TEST_CASE( rw_spinlock_test_2 )
{
    boost::thread_group threads;
    for (uint i = 0; i != thread_count; ++i)
        threads.create_thread(&test_fn);

    threads.join_all();

    BOOST_REQUIRE_EQUAL(counter, (iterations * thread_count));
}

