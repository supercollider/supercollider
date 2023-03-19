set(test_source_files
    scsynth_test.cpp
)

add_executable(scsynth_test ${test_source_files})
target_include_directories(
    scsynth_test
    PUBLIC ${CMAKE_SOURCE_DIR}/common
    PUBLIC ${CMAKE_SOURCE_DIR}/include/common
    PUBLIC ${CMAKE_SOURCE_DIR}/include/server
    PUBLIC ${CMAKE_SOURCE_DIR}/include/plugin_interface
    PUBLIC ${CMAKE_SOURCE_DIR}/server/scsynth
)
target_link_libraries(scsynth_test libscsynth boost_test)
if (PTHREADS_FOUND)
    target_link_libraries(scsynth_test ${PTHREADS_LIBRARIES})
endif()
add_test(scsynth_test_run ${EXECUTABLE_OUTPUT_PATH}/scsynth_test)
