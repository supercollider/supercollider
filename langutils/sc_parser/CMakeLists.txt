
add_library(sc_parser STATIC 
	src/ast.cpp
	src/sc_parser.cpp
	src/sc_grammar_parser.cpp
	src/sc_grammar_impl.cpp
)


target_include_directories(sc_parser PUBLIC include)
target_include_directories(sc_parser PRIVATE src)
target_link_libraries(sc_parser PUBLIC sc_lexer sc_util sc_diagnostic)
#set_target_compiler_strict_errors(sc_parser)
set_property(TARGET sc_parser PROPERTY FOLDER Libs)

if(ENABLE_TESTSUITE)
	add_subdirectory(tests)
endif()


add_subdirectory(standalone)
