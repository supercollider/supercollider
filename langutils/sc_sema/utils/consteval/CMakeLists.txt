add_executable(sc_consteval main.cpp)
target_link_libraries(sc_consteval PRIVATE sc_sema sc_parser sc_lexer sc_diagnostic Boost::thread)
set_property(TARGET sc_consteval PROPERTY FOLDER LangUtils)
