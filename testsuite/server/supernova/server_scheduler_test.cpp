#include <boost/test/unit_test.hpp>

#include <thread>
#include <boost/thread/barrier.hpp>

#include "server/server_scheduler.hpp"

using namespace nova;
using namespace boost;

BOOST_AUTO_TEST_CASE(scheduler_test_1) {
    scheduler<> sched(1);
    /*     sched(); */
}

namespace {

boost::barrier barr(2);
void thread_fn(scheduler<>* sched) {
    for (int i = 0; i != 1000; ++i)
        /* (*sched)() */;
    barr.wait();
}

}

BOOST_AUTO_TEST_CASE(scheduler_test_2) {
    scheduler<> sched(1);
    std::thread thrd(std::bind(thread_fn, &sched));
    barr.wait();
    thrd.join();
}
