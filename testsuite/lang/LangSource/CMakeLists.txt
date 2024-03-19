set(test_source_files
	sc_languageconfig_test.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/lang/LangSource)

foreach(test ${test_source_files})
	get_filename_component(test_name ${test} NAME_WE)
	add_executable(${test_name} ${test})

	target_link_libraries(${test_name} lang_boost_test libsclang)

	add_test(${test_name}_run ${EXECUTABLE_OUTPUT_PATH}/${test_name})
endforeach()

target_compile_definitions(sc_languageconfig_test PUBLIC CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
