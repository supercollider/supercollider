
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <iostream>

#include <boost/context/all.hpp>
#include <boost/exception_ptr.hpp>

#include "simple_stack_allocator.hpp"

namespace ctx = boost::context;

typedef ctx::simple_stack_allocator<
    8 * 1024 * 1024, // 8MB
    64 * 1024, // 64kB
    8 * 1024 // 8kB
>       stack_allocator;

ctx::fcontext_t fcm = 0;
ctx::fcontext_t fc = 0;
boost::exception_ptr except;

void f( intptr_t arg)
{
    try
    { throw std::runtime_error( ( char *) arg); }
    catch ( std::runtime_error const& e)
    { except = boost::current_exception(); }
    ctx::jump_fcontext( & fc, fcm, arg);
}

int main( int argc, char * argv[])
{
    stack_allocator alloc;

    void * base = alloc.allocate( stack_allocator::default_stacksize());
    fc = ctx::make_fcontext( base, stack_allocator::default_stacksize(), f);

    std::cout << "main: call start_fcontext( & fcm, fc, 0)" << std::endl;
    const char * what = "hello world";
    ctx::jump_fcontext( & fcm, fc, ( intptr_t) what);
    try
    { if ( except) boost::rethrow_exception( except); }
    catch ( std::exception const& ex)
    { std::cerr << "exception: " << ex.what() << std::endl; }

    std::cout << "main: done" << std::endl;

    return EXIT_SUCCESS;
}
