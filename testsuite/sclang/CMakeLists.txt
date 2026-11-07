file(GLOB tests *.scd)

# Makes the tests depend on sclang being built
add_test(NAME sclang.build_sclang
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target sclang --config $<CONFIG>)
set_tests_properties(sclang.build_sclang PROPERTIES FIXTURES_SETUP sclang_fixture)

foreach(test ${tests})
    get_filename_component(test_name ${test} NAME_WE)
    set(test_target sclang.crash.${test_name}_run)
    add_test(NAME ${test_target} COMMAND $<TARGET_FILE:sclang> -l ${CMAKE_BINARY_DIR}/build_sclang.cfg ${test})
    set_tests_properties(${test_target} PROPERTIES FIXTURES_REQUIRED sclang_fixture)
endforeach()

# Interpreted strings have no file, so they are compiled as --interpreted-version. Scripts in both/ run once per version
# and must pass in each, and scripts in legacy_only/ run as Legacy only. They exit with 1 if a case fails.
file(GLOB both_tests interpreted_versions/both/*.scd)
file(GLOB legacy_only_tests interpreted_versions/legacy_only/*.scd)

foreach(test ${both_tests})
    get_filename_component(test_name ${test} NAME_WE)
    foreach(version legacy 3.16)
        add_test(NAME sclang.interpreted.${version}.${test_name}_run
            COMMAND $<TARGET_FILE:sclang> -l ${CMAKE_BINARY_DIR}/build_sclang.cfg --interpreted-version ${version} ${test})
        set_tests_properties(sclang.interpreted.${version}.${test_name}_run PROPERTIES FIXTURES_REQUIRED sclang_fixture)
    endforeach()
endforeach()

foreach(test ${legacy_only_tests})
    get_filename_component(test_name ${test} NAME_WE)
    add_test(NAME sclang.interpreted.legacy.${test_name}_run
        COMMAND $<TARGET_FILE:sclang> -l ${CMAKE_BINARY_DIR}/build_sclang.cfg --interpreted-version legacy ${test})
    set_tests_properties(sclang.interpreted.legacy.${test_name}_run PROPERTIES FIXTURES_REQUIRED sclang_fixture)
endforeach()

# Text piped in with the header an editor sends, whose records give the file, position and version.
add_test(NAME sclang.piped_header_run
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/piped_header.py --sclang=$<TARGET_FILE:sclang>
        --config=${CMAKE_BINARY_DIR}/build_sclang.cfg)
set_tests_properties(sclang.piped_header_run PROPERTIES FIXTURES_REQUIRED sclang_fixture TIMEOUT 600)

# Class files with a method the compiler can't compile yet, which the library is booted with to check it throws when called.
configure_file(poisoned_methods/sclang.cfg.in ${CMAKE_BINARY_DIR}/build_sclang_poisoned_methods.cfg @ONLY)
file(GLOB poisoned_method_tests poisoned_methods/*.scd)

foreach(test ${poisoned_method_tests})
    get_filename_component(test_name ${test} NAME_WE)
    add_test(NAME sclang.poisoned_methods.${test_name}_run
        COMMAND $<TARGET_FILE:sclang> -l ${CMAKE_BINARY_DIR}/build_sclang_poisoned_methods.cfg ${test})
    set_tests_properties(sclang.poisoned_methods.${test_name}_run PROPERTIES FIXTURES_REQUIRED sclang_fixture)
endforeach()
