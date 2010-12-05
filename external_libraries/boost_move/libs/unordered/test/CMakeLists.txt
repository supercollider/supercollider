boost_additional_test_dependencies(unordered BOOST_DEPENDS test)

# GCC Compilers
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(test_compile_flags "-Wsign-promo -Wunused-parameter")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

# Intel Compiler flags
IF( ${CMAKE_CXX_COMPILER} MATCHES "icpc" )
    SET(test_compile_flags "${test_compile_flags} -strict_ansi -cxxlib-icc")  
ENDIF( ${CMAKE_CXX_COMPILER} MATCHES "icpc" )

set (swap_compile_flags "${test_compile_flags} -DBOOST_UNORDERED_SWAP_METHOD=2")

#-------------------------------------------------------------------------------
# Unordered Tests
set (unordered_tests
        fwd_set_test
        fwd_map_test
        compile_set
        compile_map
        link_test_1
         link_test_2
        simple_tests
        equivalent_keys_tests
        constructor_tests
        copy_tests
        move_tests.cpp 
        assign_tests
        insert_tests
        insert_stable_tests
        unnecessary_copy_tests
        erase_tests
        erase_equiv_tests
        find_tests
        at_tests
        bucket_tests
        load_factor_tests
        rehash_tests
        equality_tests
)
#-- Create an executable test for each source
foreach(test ${unordered_tests})
    boost_test_run(${test} "unordered/${test}.cpp"
                    COMPILE_FLAGS ${test_compile_flags} 
                    DEPENDS boost_unit_test_framework) 
endforeach(test ${unordered_tests})

#-- run the swap test
boost_test_run(swap_tests 
                COMPILE_FLAGS ${swap_compile_flags} 
                DEPENDS boost_unit_test_framework)

#-------------------------------------------------------------------------------
# Exception Tests
set (exception_tests
    constructor_exception_tests
    copy_exception_tests
    assign_exception_tests
    insert_exception_tests
    erase_exception_tests
    rehash_exception_tests
)

#-- Create an executable test for each source
foreach(test ${exception_tests})
    boost_test_run(${test} "exception/${test}.cpp"
                    COMPILE_FLAGS ${test_compile_flags} 
                    DEPENDS boost_unit_test_framework) 
endforeach(test ${unordered_tests})

#-- run the swap test
boost_test_run(swap_swap_exception_tests 
                COMPILE_FLAGS ${swap_compile_flags} 
                DEPENDS boost_unit_test_framework)
