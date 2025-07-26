set(test_sources
	slot.cpp
)

foreach(test ${test_sources})
	string(REPLACE .cpp "" test_name ${test} )
	add_executable(${test_name} ${test})

	target_link_libraries(${test_name} boost_test libsclang)

	target_include_directories(${test_name} PUBLIC 
					${boost_include_dirs}
					${CMAKE_SOURCE_DIR}/include/common
                    ${CMAKE_SOURCE_DIR}/include/lang
                    ${CMAKE_SOURCE_DIR}/include/plugin_interface
                    ${CMAKE_SOURCE_DIR}/include/server
                    ${CMAKE_SOURCE_DIR}/common

                    ${CMAKE_SOURCE_DIR}/external_libraries/boost_sync/include

                    ${CMAKE_SOURCE_DIR}/lang
                    ${CMAKE_SOURCE_DIR}/lang/LangSource
                    ${CMAKE_SOURCE_DIR}/lang/LangPrimSource
                    ${CMAKE_SOURCE_DIR}/lang/Bison

                    ${CMAKE_SOURCE_DIR}/external_libraries/nova-tt

                    ${YAMLCPP_INCLUDE_DIR}
	)

  add_test(interpreter_${test_name} ${EXECUTABLE_OUTPUT_PATH}/${test_name})
endforeach(test)
