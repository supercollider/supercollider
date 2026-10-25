include(${CMAKE_SOURCE_DIR}/cmake_modules/CompilerWarnings.cmake)

# The whole pipeline, from source files through the parser, sc_sema and sc_backend.
add_library(sc_compiler STATIC
	src/sc_compiler.cpp
)

target_include_directories(sc_compiler PUBLIC include)
target_link_libraries(sc_compiler PUBLIC sc_backend sc_sema sc_parser sc_source sc_diagnostic sc_util PRIVATE Boost::thread)
set_property(TARGET sc_compiler PROPERTY FOLDER Libs)
