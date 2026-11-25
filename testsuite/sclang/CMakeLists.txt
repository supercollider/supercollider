file(GLOB tests *.scd)

foreach(test ${tests})
    get_filename_component(test_name ${test} NAME_WE)
    set(test_target sclang__${test_name}_run)
    add_test(NAME ${test_target} COMMAND sclang -l ${CMAKE_BINARY_DIR}/build_sclang.cfg ${test})
endforeach()

# This tests that the arguments to the cmd line process get turned into utf8 correctly.
set(unicode_paths_dir "${CMAKE_CURRENT_BINARY_DIR}/ünïcödé päth")
configure_file(unicode_paths/unicode_paths.scd "${unicode_paths_dir}/unicode_paths.scd" COPYONLY)
add_test(NAME sclang__unicode_paths_run
    COMMAND sclang -l ${CMAKE_BINARY_DIR}/build_sclang.cfg "${unicode_paths_dir}/unicode_paths.scd" "ärg ü"
    WORKING_DIRECTORY "${unicode_paths_dir}")
set_tests_properties(sclang__unicode_paths_run PROPERTIES TIMEOUT 120)
