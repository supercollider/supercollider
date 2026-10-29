file(GLOB tests *.scd)

# Makes the tests depend on sclang being built
add_test(NAME build_sclang
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target sclang --config $<CONFIG>)
set_tests_properties(build_sclang PROPERTIES FIXTURES_SETUP sclang_fixture)

foreach(test ${tests})
    get_filename_component(test_name ${test} NAME_WE)
    set(test_target sclang__${test_name}_run)
    add_test(NAME ${test_target} COMMAND $<TARGET_FILE:sclang> -l ${CMAKE_BINARY_DIR}/build_sclang.cfg ${test})
    set_tests_properties(${test_target} PROPERTIES FIXTURES_REQUIRED sclang_fixture)
endforeach()

# Interpreted code is compiled as --interpreted-version, so scripts in both/ run once per version and must pass in each,
# and scripts in legacy_only/ run as Legacy only. They exit with 1 if a case fails.
file(GLOB both_tests interpreted_versions/both/*.scd)
file(GLOB legacy_only_tests interpreted_versions/legacy_only/*.scd)

foreach(test ${both_tests})
    get_filename_component(test_name ${test} NAME_WE)
    foreach(version legacy 3.16)
        add_test(NAME sclang__interpreted_${version}_${test_name}_run
            COMMAND $<TARGET_FILE:sclang> -l ${CMAKE_BINARY_DIR}/build_sclang.cfg --interpreted-version ${version} ${test})
        set_tests_properties(sclang__interpreted_${version}_${test_name}_run PROPERTIES FIXTURES_REQUIRED sclang_fixture)
    endforeach()
endforeach()

foreach(test ${legacy_only_tests})
    get_filename_component(test_name ${test} NAME_WE)
    add_test(NAME sclang__interpreted_legacy_${test_name}_run
        COMMAND $<TARGET_FILE:sclang> -l ${CMAKE_BINARY_DIR}/build_sclang.cfg --interpreted-version legacy ${test})
    set_tests_properties(sclang__interpreted_legacy_${test_name}_run PROPERTIES FIXTURES_REQUIRED sclang_fixture)
endforeach()
