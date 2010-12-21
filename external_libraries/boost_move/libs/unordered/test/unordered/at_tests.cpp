
// Copyright 2007-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/unordered_map.hpp>
#include "../helpers/test.hpp"
#include <string>

namespace at_tests {

UNORDERED_AUTO_TEST(at_tests) {
    boost::unordered_map<std::string, int> x;
    typedef boost::unordered_map<std::string, int>::iterator iterator;

    x["one"] = 1;
    x["two"] = 2;

    BOOST_CHECK(x.at("one") == 1);
    BOOST_CHECK(x.at("two") == 2);

    try {
        x.at("three");
        BOOST_ERROR("Should have thrown.");
    }
    catch(std::out_of_range) {
    }
}

}

RUN_TESTS()
