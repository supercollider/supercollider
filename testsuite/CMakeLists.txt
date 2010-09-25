set(simple_tests
    approximation_test.cpp
    buffer_manager_test.cpp
    callback_interpreter_system.cpp
    memory-pool_test.cpp
    osc_dispatcher_test.cpp
    osc_server_test.cpp
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
    ticket_scheduler_test.cpp
    timer_system_test.cpp
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

# run simple tests
foreach(test ${simple_tests})
  string(REPLACE .cpp "" test_name ${test} )
  add_executable(${test_name} ${test})
  target_link_libraries(${test_name} boost_thread boost_system boost_test)
  target_link_libraries(${test_name} oscpack tlsf)
  target_link_libraries(${test_name} pthread)
  target_link_libraries(${test_name} ${SNDFILE})
  target_link_libraries(${test_name} libsupernova)

  if(APPLE)
    target_link_libraries(${test_name} "-framework CoreAudio")
  endif()

  add_test(${test_name}_run ${EXECUTABLE_OUTPUT_PATH}/${test_name})
endforeach(test)

set_property(TARGET server_test
             APPEND
             PROPERTY COMPILE_DEFINITIONS JACK_BACKEND SUPERNOVA)
