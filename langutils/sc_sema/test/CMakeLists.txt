
add_executable(sc_sema_test main.cpp)
target_link_libraries( sc_sema_test boost_test sc_sema sc_diagnostic sc_parser)
target_include_directories(sc_sema_test PUBLIC ${boost_include_dirs})
add_test(sc_sema_test_run sc_sema_test)
set_property(TARGET sc_sema_test PROPERTY FOLDER Test/Parser)
