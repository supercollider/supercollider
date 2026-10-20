file(GLOB tests input/*.sc)

# This is how we make tests depends on executables
add_test(NAME build_standalone
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target sc_sema_standalone --config $<CONFIG>)
set_tests_properties(build_standalone PROPERTIES FIXTURES_SETUP standalone_fixture)

foreach(test ${tests})
    get_filename_component(test_name ${test} NAME_WE)
    set(test_target sc_compiler_errror_regression__${test_name}_run)

    add_test(
        NAME ${test_target} 
        COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/test.py --cmd=$<TARGET_FILE:sc_sema_standalone> --inputSCFile=${test} --outputDirectory=${CMAKE_CURRENT_BINARY_DIR}  
    )

    # Makes sure the standalone is built before.
    set_tests_properties(${test_target} PROPERTIES FIXTURES_REQUIRED standalone_fixture)
endforeach()
