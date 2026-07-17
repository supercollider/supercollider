include(${CMAKE_SOURCE_DIR}/cmake_modules/CompilerWarnings.cmake)

add_library(sc_lexer STATIC 
	src/codepoint_stream.cpp
	src/codepoint.cpp
	src/text_location.cpp
	src/source_utils.cpp
	src/tokens.cpp
)

target_include_directories(sc_lexer PUBLIC include)

target_link_libraries(sc_lexer PRIVATE utf8proc)
set_target_compiler_strict_errors(sc_lexer)
set_property(TARGET sc_lexer PROPERTY FOLDER Libs)

add_subdirectory(standalone)


if(ENABLE_TESTSUITE)
	add_subdirectory(test)
endif()
