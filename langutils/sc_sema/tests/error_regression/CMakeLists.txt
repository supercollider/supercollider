# Tests are grouped by topic, e.g., input/literals/escapes.sc is the test sc_sema_regression.literals.escapes
file(GLOB_RECURSE tests RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/input CONFIGURE_DEPENDS input/*.sc)

# This is how we make tests depends on executables
add_test(NAME build_ir_dump
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ir_dump --config $<CONFIG>)
set_tests_properties(build_ir_dump PROPERTIES FIXTURES_SETUP ir_dump_fixture)

foreach(test ${tests})
    string(REGEX REPLACE "\\.sc$" "" test_name ${test})
    string(REPLACE "/" "." test_name ${test_name})
    set(test_target sc_sema_regression.${test_name})

    add_test(
        NAME ${test_target} 
        COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/test.py --cmd=$<TARGET_FILE:ir_dump> --inputSCFile=${CMAKE_CURRENT_SOURCE_DIR}/input/${test} --outputDirectory=${CMAKE_CURRENT_BINARY_DIR}  
    )

    # Makes sure ir_dump is built before.
    set_tests_properties(${test_target} PROPERTIES FIXTURES_REQUIRED ir_dump_fixture)
endforeach()
