include(${CMAKE_SOURCE_DIR}/cmake_modules/CompilerWarnings.cmake)

# Finds the assertion comments in a regression test, see main.cpp and test.py.
add_executable(find_assertions main.cpp)
target_link_libraries(find_assertions PRIVATE sc_lexer)
set_target_compiler_strict_errors(find_assertions)
set_property(TARGET find_assertions PROPERTY FOLDER LangUtils)
