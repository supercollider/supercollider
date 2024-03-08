file(GLOB tests *.scd)

foreach(test ${tests})
    get_filename_component(test_name ${test} NAME_WE)
    set(test_target ${test_name}_run)
    add_test(NAME ${test_target} COMMAND sclang -l ${CMAKE_BINARY_DIR}/build_sclang.cfg ${test})
endforeach()

if(SC_HIDAPI)
    add_subdirectory(hidparsertest)
    message(STATUS "Building HID tests")
endif()

