#include <boost/test/unit_test.hpp>

#include "utilities/time_tag.hpp"

#include "boost/date_time/posix_time/posix_time.hpp"

using namespace nova;
using namespace boost;

BOOST_AUTO_TEST_CASE(time_tag_test_1) {
    {
        time_tag tt;
        BOOST_REQUIRE_EQUAL(tt.get_secs(), 0u);
        BOOST_REQUIRE_EQUAL(tt.get_fraction(), 0u);
    }

    {
        time_tag tt(100, 200);
        BOOST_REQUIRE_EQUAL(tt.get_secs(), 100u);
        BOOST_REQUIRE_EQUAL(tt.get_fraction(), 200u);
    }

    {
        time_tag t1(0, 0);
        time_tag t2(0, 1);
        time_tag t3(0, 1);

        BOOST_REQUIRE(t1 < t2);
        BOOST_REQUIRE(t1 != t2);
        BOOST_REQUIRE(t2 == t3);
    }

    {
        time_tag t1(0, 0);
        time_tag t2(1, 0);

        time_tag t3 = t1 + t2;
        BOOST_REQUIRE(t2 == t3);

        t3 += t2;
        BOOST_REQUIRE(t3 == time_tag(2, 0));
    }
}

BOOST_AUTO_TEST_CASE(time_tag_test_2) {
    using namespace boost::posix_time;
    time_tag first = time_tag::from_ptime(ptime(microsec_clock::universal_time()));
    time_tag next = time_tag::from_ptime(ptime(microsec_clock::universal_time()));

    BOOST_REQUIRE(first <= next);
}
