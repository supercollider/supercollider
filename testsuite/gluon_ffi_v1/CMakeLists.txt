add_library(gluon_ffi_v1_test SHARED lib.cpp)

set(CMAKE_SHARED_LIBRARY_SUFFIX ".gluon")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

target_include_directories(gluon_ffi_v1_test
	PUBLIC ${CMAKE_SOURCE_DIR}/include/gluon_ffi_interface
)

install(TARGETS gluon_ffi_v1_test)

file(GLOB tests *.scd)

foreach(test ${tests})
    get_filename_component(test_name ${test} NAME_WE)
    set(test_target ${test_name}_run)

	add_test(
		NAME ${test_target} 
		COMMAND sclang -l ${CMAKE_BINARY_DIR}/build_sclang.cfg ${test} gluon_ffi_v1_test.gluon
	)
	SET_TESTS_PROPERTIES(${test_target}
                     PROPERTIES DEPENDS gluon_ffi_v1_test DEPENDS sclang)
endforeach()

