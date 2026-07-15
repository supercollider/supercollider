include(${CMAKE_SOURCE_DIR}/cmake_modules/CompilerWarnings.cmake)

add_library(sc_diagnostic STATIC 
	src/impl.cpp
)

# todo: enable warnings

target_include_directories(sc_diagnostic PUBLIC include)
target_link_libraries(sc_diagnostic PRIVATE sc_lexer)
set_property(TARGET sc_diagnostic PROPERTY FOLDER Libs)
