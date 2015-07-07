
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <iostream>

#include <boost/coroutine/all.hpp>

int main()
{
    boost::coroutines::asymmetric_coroutine< int >::pull_type source(
        [&]( boost::coroutines::asymmetric_coroutine< int >::push_type & sink) {
            int first = 1, second = 1;
            sink( first);
            sink( second);
            for ( int i = 0; i < 8; ++i)
            {
                int third = first + second;
                first = second;
                second = third;
                sink( third);
            }
        });

    for ( auto i : source)
        std::cout << i <<  " ";

    std::cout << "\nDone" << std::endl;

    return EXIT_SUCCESS;
}
