set(simple_tests
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

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_INTEL OR CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(FATAL_ERROR "unknown compiler. supernova testsuite cannot be compiled")
endif()

find_package(Sndfile REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include/common
                    ${CMAKE_SOURCE_DIR}/include/plugin_interface
                    ${CMAKE_SOURCE_DIR}/include/server
                    ${CMAKE_SOURCE_DIR}/server/supernova
                    ${CMAKE_SOURCE_DIR}/external_libraries/boost
                    ${CMAKE_SOURCE_DIR}/external_libraries/boost_endian
                    ${CMAKE_SOURCE_DIR}/external_libraries/boost-lockfree
                    ${CMAKE_SOURCE_DIR}/external_libraries/oscpack
                    ${CMAKE_SOURCE_DIR}/external_libraries/
                    ${CMAKE_SOURCE_DIR}/external_libraries/nova-tt
                    ${CMAKE_SOURCE_DIR}/external_libraries/TLSF-2.4.6/src
                    ${SNDFILE_INCLUDE_DIR}
                    )

if (WIN32 OR APPLE)
    find_package(Portaudio)
    if(NOT PORTAUDIO_FOUND)
        message(FATAL_ERROR "Portaudio missing")
    endif()
    add_definitions(-DPORTAUDIO_BACKEND ${PORTAUDIO_DEFINITIONS})
    include_directories(${PORTAUDIO_INCLUDE_DIRS})
else()
    find_library(JACK NAMES jack)
    if (JACK)
        add_definitions(-DJACK_BACKEND)
        include_directories(${JACK_INCLUDE_DIRS})
    endif()
endif()


add_library(boost_test STATIC boost_test.cpp)

# run simple tests
foreach(test ${simple_tests})
  string(REPLACE .cpp "" test_name ${test} )
  add_executable(${test_name} ${test})

  if (Boost_FOUND)
    target_link_libraries(${test_name}  ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_TEST_LIBRARY})
  else()
    target_link_libraries(${test_name} boost_thread boost_system boost_test)
  endif()
  target_link_libraries(${test_name} oscpack tlsf)
  target_link_libraries(${test_name} ${PTHREADS_LIBRARY})
  target_link_libraries(${test_name} ${SNDFILE_LIBRARIES})
  target_link_libraries(${test_name} libsupernova)

  if(APPLE)
    target_link_libraries(${test_name} "-framework CoreAudio")
  endif()

  add_test(${test_name}_run ${EXECUTABLE_OUTPUT_PATH}/${test_name})
endforeach(test)
