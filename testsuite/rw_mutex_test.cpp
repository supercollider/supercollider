#include <boost/test/unit_test.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "../source/utilities/rw_mutex.hpp"

struct foo:
    virtual public nova::rw_lockable
{
};

struct bar:
    public foo
{
};

BOOST_AUTO_TEST_CASE( rw_mutex_test )
{
    using namespace nova;
    rw_mutex mut;

    mut.lock_shared();
    mut.lock_shared();
    BOOST_REQUIRE_EQUAL( mut.try_lock(), false );
    BOOST_REQUIRE_EQUAL( mut.try_lock_shared(), true );
    {
        scoped_read_lock lock(mut);
    }
    mut.unlock_shared();
    mut.unlock_shared();
    mut.unlock_shared();

    BOOST_REQUIRE_EQUAL( mut.try_lock(), true );
    mut.lock();
    mut.lock();
    BOOST_REQUIRE_EQUAL( mut.try_lock_shared(), true );
    mut.unlock_shared();
    mut.unlock();
    mut.unlock();
    mut.unlock();

    {
        foo f;
        scoped_read_lock lock(f);
    }

    {
        const bar g;
        scoped_read_lock lock2(&g);
    }

    {
        boost::shared_ptr<foo> f(new foo());
        scoped_read_lock lock(f);
    }

}

namespace
{
    using namespace nova;
    rw_mutex mut;

    void sleep ()
    {
        boost::thread::yield();
/*        boost::xtime time;
        xtime_get(&time, 0);
        time.nsec += 1000;
        boost::thread::sleep(time);*/
    }

    void test_fn(void)
    {
        static rw_mutex mut;

        for (int i = 0; i != 200000; ++i)
        {
            /* recursive read locks */
            {
                scoped_read_lock lock(&mut);
                sleep();
                {
                    scoped_read_lock lock(mut);
                    sleep();
                }
                sleep();
            }

            /* recursive write locks */
            {
                scoped_write_lock lock(mut);
                sleep();
                {
                    scoped_write_lock lock(mut);
                    sleep();
                }
                sleep();
            }

            /* nested read / write locks */
            {
                scoped_write_lock lock(mut);
                sleep();
                {
                    scoped_read_lock lock(mut);
                    sleep();
                }
                sleep();
            }
        }
    }

}
#if 0
BOOST_AUTO_TEST_CASE( rw_mutex_test_2 )
{
    boost::thread_group threads;
    threads.create_thread(&test_fn);
    threads.create_thread(&test_fn);
    threads.create_thread(&test_fn);
    threads.create_thread(&test_fn);
    threads.create_thread(&test_fn);
    threads.create_thread(&test_fn);
    threads.create_thread(&test_fn);
    threads.create_thread(&test_fn);

    threads.join_all();
}
#endif
