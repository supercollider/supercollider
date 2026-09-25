include(${CMAKE_SOURCE_DIR}/cmake_modules/CompilerWarnings.cmake)

add_executable(sc_lexer_standalone main.cpp)
target_link_libraries(sc_lexer_standalone PRIVATE sc_lexer)
set_target_compiler_strict_errors(sc_lexer_standalone)
set_property(TARGET sc_lexer_standalone PROPERTY FOLDER Util)
