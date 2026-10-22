add_executable(ir_dump main.cpp )
target_link_libraries(ir_dump PRIVATE sc_sema sc_parser sc_lexer sc_diagnostic Boost::thread)
set_property(TARGET ir_dump PROPERTY FOLDER LangUtils)
