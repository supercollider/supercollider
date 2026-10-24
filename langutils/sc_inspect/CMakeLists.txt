add_executable(sc_inspect main.cpp )
target_link_libraries(sc_inspect PRIVATE sc_describe sc_backend sc_sema sc_parser sc_lexer sc_diagnostic Boost::thread)
set_property(TARGET sc_inspect PROPERTY FOLDER LangUtils)
