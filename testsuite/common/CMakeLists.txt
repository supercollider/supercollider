set(test_source_files
    rgen_test.cpp
)

foreach(test_file ${test_source_files})
    string(REPLACE .cpp "" test_name ${test_file} )
    add_executable(${test_name} ${test_file})
    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/include/common
        ${CMAKE_SOURCE_DIR}/include/server
        ${CMAKE_SOURCE_DIR}/include/plugin_interface
        ${CMAKE_SOURCE_DIR}/server/scsynth
    )
    target_link_libraries(${test_name} PRIVATE libscsynth boost_test)
    if (PTHREADS_FOUND)
        target_link_libraries(${test_name} PRIVATE ${PTHREADS_LIBRARIES})
    endif()

    add_test(${test_name}_run ${EXECUTABLE_OUTPUT_PATH}/${test_name})

    set_property(TARGET ${test_name} PROPERTY FOLDER Test/common)
endforeach()
