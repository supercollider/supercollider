#include "utilities/string_resource.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <string>

using namespace std;

class local_string:
    public std::string
{
public:
    template<typename arg>
    local_string(arg const & a):
        std::string(a)
    {
    }

    local_string()
    {
    }
};

typedef nova::string_resource<local_string> test_string_resource;

BOOST_AUTO_TEST_CASE( string_resource_test )
{
    using namespace nova;

    test_string_resource r1("foo");
    test_string_resource r2("foo");
    test_string_resource r3("bar");

    {
        test_string_resource r2("foo");

        BOOST_REQUIRE_EQUAL(r2, local_string("foo"));
        BOOST_REQUIRE_EQUAL(r2, "foo");

        BOOST_REQUIRE_EQUAL(boost::lexical_cast<local_string>(r2), "foo");

        BOOST_REQUIRE(r2 != local_string("bar"));
        BOOST_REQUIRE(r2 != "bar");

        test_string_resource r3(r2);
        BOOST_REQUIRE_EQUAL(r2, r3);

        r2.reset("bar");
        r3.reset(r2);
        BOOST_REQUIRE_EQUAL(r2, r3);
    }

    test_string_resource r0;
    BOOST_REQUIRE_EQUAL(r0, "");
    BOOST_REQUIRE_EQUAL(r0.as_str().size(), 0u);

}

