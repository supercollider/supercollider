add_executable(sc_format_tests tests.cpp)
target_link_libraries(sc_format_tests boost_test sc_format)
target_include_directories(sc_format_tests PUBLIC ${boost_include_dirs})
add_test(langutils.sc_format_tests_run sc_format_tests)
set_property(TARGET sc_format_tests PROPERTY FOLDER Test/Format)

if(TARGET sc_format_cli)
    # Builds sc_format before the tests that run it, as ctest doesn't build anything.
    add_test(
        NAME langutils.sc_format_build
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target sc_format_cli --config $<CONFIG>
    )
    set_tests_properties(langutils.sc_format_build PROPERTIES FIXTURES_SETUP sc_format_fixture)

    # Over the class library, the examples, and the testsuite, with each options file in styles/: formatting keeps every
    # token, formatting twice changes nothing, and with every option keep only line endings change.
    foreach(style keep all other indent spacing lines parameters closures lists)
        add_test(
            NAME langutils.sc_format_verify_${style}
            COMMAND sc_format_cli --verify --config ${CMAKE_CURRENT_SOURCE_DIR}/styles/${style}.sc-format.scd
                    ${CMAKE_SOURCE_DIR}/SCClassLibrary ${CMAKE_SOURCE_DIR}/examples ${CMAKE_SOURCE_DIR}/testsuite
        )
        set_tests_properties(langutils.sc_format_verify_${style} PROPERTIES FIXTURES_REQUIRED sc_format_fixture)
    endforeach()

    # The repository's sclang files follow their options files (.sc-format.scd at the root, the style guide). The
    # pre-commit hook formats them when it finds sc_format; this catches what it didn't.
    add_test(
        NAME langutils.sc_format_repo_check
        COMMAND sc_format_cli --check ${CMAKE_SOURCE_DIR}/.sc-format.scd ${CMAKE_SOURCE_DIR}/SCClassLibrary
                ${CMAKE_SOURCE_DIR}/testsuite ${CMAKE_SOURCE_DIR}/examples ${CMAKE_SOURCE_DIR}/platform
                ${CMAKE_SOURCE_DIR}/tools ${CMAKE_SOURCE_DIR}/icons
    )
    set_tests_properties(langutils.sc_format_repo_check PROPERTIES FIXTURES_REQUIRED sc_format_fixture)

    # Each file in input/, formatted with the options of the nearest .sc-format.scd above it, gives the file at the same
    # path in expected/, or its input unchanged when there is no file there, e.g., input/indent/comments.scd is
    # langutils.sc_format_golden.indent.comments. After a change, build the sc_format_golden_override target and review the
    # diff of expected/.
    file(GLOB_RECURSE golden_tests RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/input CONFIGURE_DEPENDS input/*.sc input/*.scd)
    foreach(test ${golden_tests})
        get_filename_component(file_name ${test} NAME)
        if(file_name STREQUAL ".sc-format.scd")
            continue()
        endif()
        string(REGEX REPLACE "\\.scd?$" "" test_name ${test})
        string(REPLACE "/" "." test_name ${test_name})
        add_test(
            NAME langutils.sc_format_golden.${test_name}
            COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/test.py --cmd=$<TARGET_FILE:sc_format_cli> ${test}
        )
        set_tests_properties(langutils.sc_format_golden.${test_name} PROPERTIES FIXTURES_REQUIRED sc_format_fixture)
    endforeach()

    add_custom_target(sc_format_golden_override
        COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/test.py --cmd=$<TARGET_FILE:sc_format_cli> --override
        DEPENDS sc_format_cli
        COMMENT "Writes the results of sc_format's golden tests to expected/"
    )
    set_property(TARGET sc_format_golden_override PROPERTY FOLDER Util)
endif()

# A page showing each golden test's input beside its expected output.
add_custom_target(sc_format_golden_html
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/test.py --html=${CMAKE_CURRENT_BINARY_DIR}/sc_format_golden.html
    COMMENT "Writes a page showing sc_format's golden tests to ${CMAKE_CURRENT_BINARY_DIR}/sc_format_golden.html"
)
set_property(TARGET sc_format_golden_html PROPERTY FOLDER Util)
