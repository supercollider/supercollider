#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>

#define HAVE_SEMAPHORE_H
#include "utilities/ticket_scheduler.hpp"


namespace
{

int thread_count = 5;

nova::ticket_scheduler sched;

void test_fn(std::size_t check)
{
    static std::size_t count = 0;

    BOOST_REQUIRE_EQUAL(count, check);
    ++count;
}

const int iterations = 150;


void runme_nonblocking(void)
{
    for (int i = 0; i != iterations; ++i)
    {
        std::size_t ticket = sched.get_ticket();

        boost::thread::yield();

        for(;;)
        {
            if (sched.run_nonblocking(ticket, boost::bind(test_fn, ticket)))
                break;
            else
                boost::thread::yield();
        }
    }
}


void runme(void)
{
    for (int i = 0; i != iterations; ++i)
    {
        std::size_t ticket = sched.get_ticket();

        boost::thread::yield();

        sched.run(ticket, boost::bind(test_fn, ticket));
    }
}

}


BOOST_AUTO_TEST_CASE( ticket_scheduler_test )
{
    {
        boost::thread_group threads;

        for (int i = 0; i != thread_count; ++i)
        {
            threads.create_thread(runme_nonblocking);
        }

        threads.join_all();
    }

    {
        boost::thread_group threads;

        for (int i = 0; i != thread_count; ++i)
        {
            threads.create_thread(runme);
        }

        threads.join_all();
    }
}
