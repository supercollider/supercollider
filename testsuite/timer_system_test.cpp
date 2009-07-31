#include <boost/test/unit_test.hpp>

#include "../source/utilities/timer_system.hpp"
#include "../source/utilities/time_tag.hpp"

using namespace nova;

namespace
{

class timer_callback:
    public timer_system<time_tag>::timer_callback
{
public:
    timer_callback(time_tag const & tt = time_tag()):
        timer_system<time_tag>::timer_callback(tt)
    {}

    void run(void)
    {}
};

}

BOOST_AUTO_TEST_CASE( timer_system_test_1 )
{
    timer_system<time_tag> ts;

    BOOST_REQUIRE_EQUAL( (ts.check_next_interval(time_tag(0, 0))), false);

    ts.add_callback(new timer_callback());

    ts.tick();
}
