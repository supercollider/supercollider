
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <boost/context/all.hpp>
#include <boost/cstdint.hpp>
#include <boost/program_options.hpp>

#include "../bind_processor.hpp"
#include "../clock.hpp"
#include "../cycle.hpp"
#include "../../example/simple_stack_allocator.hpp"

typedef boost::context::simple_stack_allocator<
            8 * 1024 * 1024, 64 * 1024, 8 * 1024
        >                                       stack_allocator;

bool preserve_fpu = false;
boost::uint64_t jobs = 1000;
boost::context::fcontext_t fcm = 0;
boost::context::fcontext_t fc = 0;

#if __cplusplus >= 201103L
boost::context::execution_context * mctx = nullptr;
#endif

static void foo( intptr_t) {
    while ( true) {
        boost::context::jump_fcontext( & fc, fcm, 7, preserve_fpu);
    }
}

#if __cplusplus >= 201103L
static void bar() {
    while ( true) {
        mctx->resume();
    }
}
#endif

duration_type measure_time_fc() {
    // cache warum-up
    boost::context::jump_fcontext( & fcm, fc, 7, preserve_fpu);
        
    time_point_type start( clock_type::now() );
    for ( std::size_t i = 0; i < jobs; ++i) {
        boost::context::jump_fcontext( & fcm, fc, 7, preserve_fpu);
    }
    duration_type total = clock_type::now() - start;
    total -= overhead_clock(); // overhead of measurement
    total /= jobs;  // loops
    total /= 2;  // 2x jump_fcontext

    return total;
}

#if __cplusplus >= 201103L
duration_type measure_time_ec() {
    boost::context::execution_context ctx( boost::context::execution_context::current() );
    mctx = & ctx;
    // cache warum-up
    boost::context::fixedsize_stack alloc;
    boost::context::execution_context ectx( alloc, bar);
    ectx.resume();
        
    time_point_type start( clock_type::now() );
    for ( std::size_t i = 0; i < jobs; ++i) {
        ectx.resume();
    }
    duration_type total = clock_type::now() - start;
    total -= overhead_clock(); // overhead of measurement
    total /= jobs;  // loops
    total /= 2;  // 2x jump_fcontext

    return total;
}
#endif

#ifdef BOOST_CONTEXT_CYCLE
cycle_type measure_cycles_fc() {
    // cache warum-up
    boost::context::jump_fcontext( & fcm, fc, 7, preserve_fpu);
        
    cycle_type start( cycles() );
    for ( std::size_t i = 0; i < jobs; ++i) {
        boost::context::jump_fcontext( & fcm, fc, 7, preserve_fpu);
    }
    cycle_type total = cycles() - start;
    total -= overhead_cycle(); // overhead of measurement
    total /= jobs;  // loops
    total /= 2;  // 2x jump_fcontext

    return total;
}

# if __cplusplus >= 201103L
cycle_type measure_cycles_ec() {
    boost::context::execution_context ctx( boost::context::execution_context::current() );
    mctx = & ctx;
    // cache warum-up
    boost::context::fixedsize_stack alloc;
    boost::context::execution_context ectx( alloc, bar);
    ectx.resume();
        
    cycle_type start( cycles() );
    for ( std::size_t i = 0; i < jobs; ++i) {
        ectx.resume();
    }
    cycle_type total = cycles() - start;
    total -= overhead_cycle(); // overhead of measurement
    total /= jobs;  // loops
    total /= 2;  // 2x jump_fcontext

    return total;
}
# endif
#endif

int main( int argc, char * argv[])
{
    try
    {
        bind_to_processor( 0);

        boost::program_options::options_description desc("allowed options");
        desc.add_options()
            ("help", "help message")
            ("fpu,f", boost::program_options::value< bool >( & preserve_fpu), "preserve FPU registers")
            ("jobs,j", boost::program_options::value< boost::uint64_t >( & jobs), "jobs to run");

        boost::program_options::variables_map vm;
        boost::program_options::store(
                boost::program_options::parse_command_line(
                    argc,
                    argv,
                    desc),
                vm);
        boost::program_options::notify( vm);

        if ( vm.count("help") ) {
            std::cout << desc << std::endl;
            return EXIT_SUCCESS;
        }
 
        stack_allocator stack_alloc;
        fc = boost::context::make_fcontext(
                stack_alloc.allocate( stack_allocator::default_stacksize() ),
                stack_allocator::default_stacksize(),
                foo);

        boost::uint64_t res = measure_time_fc().count();
        std::cout << "fcontext_t: average of " << res << " nano seconds" << std::endl;
# if __cplusplus >= 201103L
        res = measure_time_ec().count();
        std::cout << "execution_context: average of " << res << " nano seconds" << std::endl;
# endif
#ifdef BOOST_CONTEXT_CYCLE
        res = measure_cycles_fc();
        std::cout << "fcontext_t: average of " << res << " cpu cycles" << std::endl;
# if __cplusplus >= 201103L
        res = measure_cycles_ec();
        std::cout << "execution_context: average of " << res << " cpu cycles" << std::endl;
# endif
#endif

        return EXIT_SUCCESS;
    }
    catch ( std::exception const& e)
    { std::cerr << "exception: " << e.what() << std::endl; }
    catch (...)
    { std::cerr << "unhandled exception" << std::endl; }
    return EXIT_FAILURE;
}
