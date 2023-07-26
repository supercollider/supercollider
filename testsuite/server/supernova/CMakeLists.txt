set(simple_tests
    buffer_manager_test.cpp
    callback_interpreter_system.cpp
    memory-pool_test.cpp
    server_dsp_thread_queue_test.cpp
    server_dsp_thread_test.cpp
    server_node_graph_test.cpp
    server_scheduler_test.cpp
    server_synth_factory_test.cpp
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

# run simple tests
foreach(test ${simple_tests})
  get_filename_component(test_name ${test} NAME_WE)
  add_executable(${test_name} ${test})

  target_link_libraries(${test_name} server_boost_test libsupernova boost_thread_lib)

  add_test(${test_name}_run ${EXECUTABLE_OUTPUT_PATH}/${test_name})
endforeach(test)
