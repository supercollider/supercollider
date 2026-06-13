set(test_sources
	slot.cpp
)

# These are tests that will fail to compile.
set(negative_tests
	pyrslot_int64.cpp
	pyrslot_int8.cpp
	pyrslot_ptr.cpp
	pyrslot_enum.cpp
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
	)

  add_test(interpreter__${test_name} ${test_name})
  set_property(TARGET ${test_name} PROPERTY FOLDER Test/Interpreter)
endforeach(test)





foreach(test ${negative_tests})
	string(REPLACE .cpp "" test_name ${test} )
	add_executable(${test_name} ${test})

	# Don't build these normally
	set_target_properties(${test_name} PROPERTIES
                      EXCLUDE_FROM_ALL TRUE
                      EXCLUDE_FROM_DEFAULT_BUILD TRUE)

	target_link_libraries(${test_name} boost_test libsclang)

	target_include_directories(${test_name} PUBLIC 
					${boost_include_dirs}
					${CMAKE_SOURCE_DIR}/include/common
                    ${CMAKE_SOURCE_DIR}/include/lang
                    ${CMAKE_SOURCE_DIR}/include/plugin_interface
                    ${CMAKE_SOURCE_DIR}/include/server
                    ${CMAKE_SOURCE_DIR}/common

                    ${CMAKE_SOURCE_DIR}/lang
                    ${CMAKE_SOURCE_DIR}/lang/LangSource
                    ${CMAKE_SOURCE_DIR}/lang/LangPrimSource
	)

	add_test(NAME failing_${test_name}
         COMMAND ${CMAKE_COMMAND} --build . --target ${test_name} --config $<CONFIGURATION>
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

	set_tests_properties(failing_${test_name} PROPERTIES WILL_FAIL TRUE)
	set_property(TARGET ${test_name} PROPERTY FOLDER Test/Interpreter)
endforeach(test)
