
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
    std::cout << "calling caller..." << std::endl;
    ctx::jump_fcontext( & fc, fcm, arg);
    try {
        try {
            throw std::runtime_error("mycoro exception");
        } catch(const std::exception& e) {
            std::cout << "calling caller in the catch block..." << std::endl;
            ctx::jump_fcontext( & fc, fcm, arg);
            std::cout << "rethrowing mycoro exception..." << std::endl;
            throw;
        }
    } catch (const std::exception& e) {
        std::cout << "mycoro caught: " << e.what() << std::endl;
    }
    std::cout << "exiting mycoro..." << std::endl;
}

int main( int argc, char * argv[])
{
    stack_allocator alloc;

    void * base = alloc.allocate( stack_allocator::default_stacksize());
    fc = ctx::make_fcontext( base, stack_allocator::default_stacksize(), f);

    ctx::jump_fcontext( & fcm, fc, ( intptr_t) 0);
    try {
        try {
            throw std::runtime_error("main exception");
        } catch( std::exception const& e) {
            std::cout << "calling callee in the catch block..." << std::endl;
            ctx::jump_fcontext( & fcm, fc, ( intptr_t) 0);
            std::cout << "rethrowing main exception..." << std::endl;
            throw;
        }
    } catch ( std::exception const& e) {
        std::cout << "main caught: " << e.what() << std::endl;
    }
    std::cout << "calling callee one last time..." << std::endl;
    ctx::jump_fcontext( & fcm, fc, ( intptr_t) 0);
    std::cout << "exiting main..." << std::endl;

    return EXIT_SUCCESS;
}
