include(${CMAKE_SOURCE_DIR}/cmake_modules/CompilerWarnings.cmake)

add_library(sc_sema STATIC 
	#
	src/compiler.cpp
	src/ast2ir/expr.cpp
	src/ast2ir/declare_classes.cpp
	src/ast2ir/region.cpp
)

target_include_directories(sc_sema PUBLIC include)
target_include_directories(sc_sema PRIVATE src)
target_link_libraries(sc_sema PRIVATE sc_lexer sc_parser sc_util)
set_target_compiler_strict_errors(sc_sema)
set_property(TARGET sc_sema PROPERTY FOLDER Libs)

if(ENABLE_TESTSUITE)
	#add_subdirectory(tests)
endif()


add_subdirectory(standalone)
