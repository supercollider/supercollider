
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <iostream>

#include <boost/coroutine/all.hpp>

#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/iterator_range.hpp>
#include <iostream>

int main()
{
    boost::coroutines::asymmetric_coroutine<int>::pull_type c(
        [](boost::coroutines::asymmetric_coroutine<int>::push_type& yield) {
            for (int i = 0; i < 5; ++i) {
                yield(i);
            }
        });
    auto crange = boost::make_iterator_range(boost::begin(c), boost::end(c));
    for (auto n : crange) {
        std::cout << n << std::endl;
    }
}
