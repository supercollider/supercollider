set(SCLANG ${CMAKE_CURRENT_BINARY_DIR}/../../lang/sclang${CMAKE_EXECUTABLE_SUFFIX}
							-l ${CMAKE_CURRENT_BINARY_DIR}/../../build_sclang.cfg)

file(GLOB tests *.scd)

foreach(test ${tests})
	add_test(${test}_run ${SCLANG} ${test})
endforeach()
