set(test_source_files
	sc_languageconfig_test.cpp
)

configure_file(sc_languageconfig_test.h.in sc_languageconfig_test.h @ONLY)

include_directories(${CMAKE_SOURCE_DIR}/lang/LangSource)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
foreach(test ${test_source_files})
	get_filename_component(test_name ${test} NAME_WE)
	add_executable(${test_name} ${test})

	target_link_libraries(${test_name} lang_boost_test libsclang)

	add_test(${test_name}_run ${EXECUTABLE_OUTPUT_PATH}/${test_name})
endforeach()
