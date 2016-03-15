set(simple_tests
    buffer_manager_test.cpp
    callback_interpreter_system.cpp
    memory-pool_test.cpp
    server_dsp_thread_queue_test.cpp
    server_dsp_thread_test.cpp
    server_node_graph_test.cpp
    server_scheduler_test.cpp
    server_synth_factory_test.cpp
    server_test.cpp
    simple_pool_test.cpp
    sized_array_test.cpp
    sndfile_backend_test.cpp
    static_pool_test.cpp
    timetag_test.cpp
)

if(!APPLE)
    set(simple_tests
        ${simple_tests}
        static_allocator_test.cpp
        tl_allocator_test.cpp
    )
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(simple_tests ${simple_tests}
        perf_counter_test.cpp
    )
endif()

add_library(boost_test STATIC boost_test.cpp)
target_include_directories(boost_test PUBLIC ${CMAKE_SOURCE_DIR}/external_libraries/boost)

# run simple tests
foreach(test ${simple_tests})
  string(REPLACE .cpp "" test_name ${test} )
  add_executable(${test_name} ${test})

  target_link_libraries(${test_name} libsupernova boost_test boost_thread)

  add_test(${test_name}_run ${EXECUTABLE_OUTPUT_PATH}/${test_name})
endforeach(test)
