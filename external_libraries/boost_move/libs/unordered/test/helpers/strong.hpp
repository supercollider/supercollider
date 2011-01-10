
// Copyright 2005-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_UNORDERED_TEST_HELPERS_STRONG_HEADER)
#define BOOST_UNORDERED_TEST_HELPERS_STRONG_HEADER

#include <boost/config.hpp>
#include <iterator>
#include "./metafunctions.hpp"
#include "./equivalent.hpp"
#include "./list.hpp"
#include "./exception_test.hpp"

namespace test
{
    template <class X>
    class strong
    {
        typedef test::list<BOOST_DEDUCED_TYPENAME X::value_type> values_type;
        values_type values_;
    public:
        void store(X const& x) {
            DISABLE_EXCEPTIONS;
            values_.clear();
            values_.insert(x.cbegin(), x.cend());
        }

        void test(X const& x) const {
            if(!(x.size() == values_.size() &&
                    std::equal(x.cbegin(), x.cend(), values_.begin(),
                        test::equivalent)))
                BOOST_ERROR("Strong exception safety failure.");
        }
    };
}

#endif
