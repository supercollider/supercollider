
add_executable(sc_parser_standalone main.cpp)
target_link_libraries(sc_parser_standalone PRIVATE sc_parser)
set_property(TARGET sc_parser_standalone PROPERTY FOLDER Util)
