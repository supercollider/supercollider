#include <boost/test/unit_test.hpp>

#include "utilities/osc_dispatcher.hpp"
#include <memory>

using namespace nova;
using namespace std;

namespace
{
struct osc_test_responder:
    public osc_responder
{
    osc_test_responder(void):
        i(0)
    {}

    void run(osc::ReceivedMessageArgumentIterator begin,
             osc::ReceivedMessageArgumentIterator const & end)
    {
        ++i;
    }

    int i;
};

}
BOOST_AUTO_TEST_CASE( osc_dispatcher_test_1 )
{
    osc_dispatcher dispatcher;

    std::auto_ptr<osc_test_responder> resp (new osc_test_responder());
    dispatcher.add_responder("foo", resp.get());
    dispatcher.remove_responder("foo", resp.get());
}

BOOST_AUTO_TEST_CASE( osc_pattern_test )
{
    BOOST_REQUIRE_EQUAL(osc_dispatcher::is_pattern("/abc?de"), true);
    BOOST_REQUIRE_EQUAL(osc_dispatcher::is_pattern("/abc*de"), true);
    BOOST_REQUIRE_EQUAL(osc_dispatcher::is_pattern("/ab*?de"), true);
    BOOST_REQUIRE_EQUAL(osc_dispatcher::is_pattern("/abcde"), false);

    BOOST_REQUIRE_EQUAL(osc_dispatcher::match_pattern("/abcde", "/abcde"), true);
    BOOST_REQUIRE_EQUAL(osc_dispatcher::match_pattern("/abcde", "/abcdef"), false);
    BOOST_REQUIRE_EQUAL(osc_dispatcher::match_pattern("/abcdef", "/abcde"), false);

    BOOST_REQUIRE_EQUAL(osc_dispatcher::match_pattern("/ab?de", "/abcde"), true);
    BOOST_REQUIRE_EQUAL(osc_dispatcher::match_pattern("/ab?de", "/abcdf"), false);
    BOOST_REQUIRE_EQUAL(osc_dispatcher::match_pattern("/ab?de", "/abcdef"), false);

    BOOST_REQUIRE_EQUAL(osc_dispatcher::match_pattern("/ab*de", "/abcde"), true);
    BOOST_REQUIRE_EQUAL(osc_dispatcher::match_pattern("/ab*de", "/ab"), false);
    BOOST_REQUIRE_EQUAL(osc_dispatcher::match_pattern("/ab*de", "/abxyz"), false);
    BOOST_REQUIRE_EQUAL(osc_dispatcher::match_pattern("/ab*de", "/abxdez"), false);
    BOOST_REQUIRE_EQUAL(osc_dispatcher::match_pattern("/ab*de*", "/abxdez"), true);
    BOOST_REQUIRE_EQUAL(osc_dispatcher::match_pattern("/ab*de*", "/abxedz"), false);
    BOOST_REQUIRE_EQUAL(osc_dispatcher::match_pattern("/*de*", "/abcdefghi"), true);
    BOOST_REQUIRE_EQUAL(osc_dispatcher::match_pattern("/*de*", "/abcedfghi"), false);
}


BOOST_AUTO_TEST_CASE( osc_dispatcher_test_2 )
{
    osc_dispatcher dispatcher;

    std::auto_ptr<osc_test_responder> resp (new osc_test_responder());
    dispatcher.add_responder("/foo", resp.get());
    dispatcher.add_responder("/fpp", resp.get());

    dispatcher.handle_packet("/foo\0\0\0", 8);
    dispatcher.handle_packet("/fo*\0\0\0", 8);

    BOOST_REQUIRE_EQUAL(resp->i, 2);
    dispatcher.remove_responder("/foo", resp.get());
    dispatcher.remove_responder("/fpp", resp.get());
}
