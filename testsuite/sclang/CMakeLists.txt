file(GLOB tests *.scd)

foreach(test ${tests})
    get_filename_component(test_name ${test} NAME_WE)
    set(test_target sclang__${test_name}_run)
    add_test(NAME ${test_target} COMMAND sclang -l ${CMAKE_BINARY_DIR}/build_sclang.cfg ${test})
endforeach()

add_test(NAME sclang__script_args_run
    COMMAND sclang -l ${CMAKE_BINARY_DIR}/build_sclang.cfg ${CMAKE_CURRENT_SOURCE_DIR}/args/script_args.scd --foo=1 -b bar)
