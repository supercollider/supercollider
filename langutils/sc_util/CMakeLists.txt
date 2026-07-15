include(${CMAKE_SOURCE_DIR}/cmake_modules/CompilerWarnings.cmake)

add_library(sc_util INTERFACE 
	include/sc_util/strong_index.hpp
	include/sc_util/type_set_index.hpp
	include/sc_util/typed_graph_node.hpp
	include/sc_util/typed_graph.hpp
	include/sc_util/overload.hpp
)


# todo: enable warnings

target_include_directories(sc_util INTERFACE include)
set_property(TARGET sc_util PROPERTY FOLDER Libs)
