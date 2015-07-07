
//          Copyright Keld Helsgaun 2000, Oliver Kowalke 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <cstddef>
#include <iostream>
#include <vector>

#include <boost/coroutine/all.hpp>

typedef boost::coroutines::symmetric_coroutine< void >  coro_t;

std::vector< int > merge( std::vector< int > const& a, std::vector< int > const& b)
{
    std::vector< int > c;
    std::size_t idx_a = 0, idx_b = 0;
    coro_t::call_type * other_a = 0, * other_b = 0;

    coro_t::call_type coro_a(
        [&]( coro_t::yield_type & yield) {
            while ( idx_a < a.size() )
            {
                if ( b[idx_b] < a[idx_a])
                    yield( * other_b);
                c.push_back(a[idx_a++]);
            }
            while ( c.size() < a.size() + b.size())
                c.push_back( b[idx_b]); 
        });

    coro_t::call_type coro_b(
        [&]( coro_t::yield_type & yield) {
            while ( idx_b < b.size() )
            {
                if ( a[idx_a] < b[idx_b])
                    yield( * other_a);
                c.push_back(b[idx_b++]);
            }
            while ( c.size() < ( a.size() + b.size() ) )
                c.push_back( a[idx_a]); 
        });


    other_a = & coro_a;
    other_b = & coro_b;

    coro_a();

    return c;
}

void print( std::string const& name, std::vector< int > const& v)
{
    std::cout << name << " : ";
    for ( auto itm : v)
    { std::cout << itm << " "; }
    std::cout << "\n";
}

int main( int argc, char * argv[])
{
    std::vector< int > a = { 1, 5, 6, 10 };
    print( "a", a);

    std::vector< int > b = { 2, 4, 7, 8, 9, 13 };
    print( "b", b);

    std::vector< int > c = merge( a, b);
    print( "c", c);

    std::cout << "Done" << std::endl;

    return EXIT_SUCCESS;
}
