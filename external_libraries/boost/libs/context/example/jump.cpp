
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <cstring>
#include <iostream>

#include <boost/assert.hpp>
#include <boost/context/all.hpp>

#include "simple_stack_allocator.hpp"

namespace ctx = boost::context;

typedef ctx::simple_stack_allocator<
    8 * 1024 * 1024, // 8MB
    64 * 1024, // 64kB
    8 * 1024 // 8kB
>       stack_allocator;

ctx::fcontext_t fcm = 0;
ctx::fcontext_t fc1 = 0;

void f1( intptr_t)
{
    std::cout << "f1: entered" << std::endl;
    ctx::jump_fcontext( & fc1, fcm, 0, false);
}

int main( int argc, char * argv[])
{
    stack_allocator alloc;

    void * base1 = alloc.allocate( stack_allocator::default_stacksize());
    fc1 = ctx::make_fcontext( base1, stack_allocator::default_stacksize(), f1);

    std::cout << "main: call start_fcontext( & fcm, fc1, 0)" << std::endl;
    ctx::jump_fcontext( & fcm, fc1, 0, false);

    std::cout << "main: done" << std::endl;

    return EXIT_SUCCESS;
}
