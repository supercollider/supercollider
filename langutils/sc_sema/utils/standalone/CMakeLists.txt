add_executable(sc_sema_standalone main.cpp )
target_link_libraries(sc_sema_standalone PRIVATE sc_sema sc_parser sc_lexer sc_diagnostic Boost::thread)
set_property(TARGET sc_sema_standalone PROPERTY FOLDER LangUtils)
