include(${CMAKE_SOURCE_DIR}/cmake_modules/CompilerWarnings.cmake)

add_library(sc_sema STATIC 
	#
	src/compiler.cpp
	src/tables.cpp
	src/const_expr.cpp
	src/ir.cpp
	src/common_class.cpp
	src/common_symbols.cpp
	src/known_methods.cpp
	src/compile_consteval.cpp
	src/intrinsic_constraint_checker.cpp
	src/inheritance_edges.cpp
	src/ast2ir/expr/dispatch.cpp
	src/ast2ir/expr/blocks.cpp
	src/ast2ir/expr/collections.cpp
	src/ast2ir/expr/generators.cpp
	src/ast2ir/expr/literals.cpp
	src/ast2ir/expr/messages.cpp
	src/ast2ir/expr/ranges.cpp
	src/ast2ir/expr/structure.cpp
	src/ast2ir/expr/variables.cpp
	src/ast2ir/expr/unimplemented.cpp
	src/ast2ir/declare_classes.cpp
	src/ast2ir/region.cpp
	src/ast2ir/consteval.cpp
	src/ast2ir/ast2ir_base.cpp
	src/ast2ir/method.cpp
	src/ast2ir/arguments.cpp
	src/ast2ir/body.cpp
	src/ast2ir/literal_synthesis.cpp
)

target_include_directories(sc_sema PUBLIC include)
target_include_directories(sc_sema PRIVATE src)
target_link_libraries(sc_sema PRIVATE sc_lexer sc_parser sc_util Boost::thread)
#set_target_compiler_strict_errors(sc_sema)
set_property(TARGET sc_sema PROPERTY FOLDER Libs)

if(ENABLE_TESTSUITE)
	add_subdirectory(tests)
endif()


add_subdirectory(utils/ir_dump)
add_subdirectory(utils/consteval)
