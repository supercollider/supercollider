if (NOT APPLE AND NOT WIN32)
	# on apple we leave this undefined so it defaults to .scx (in the code)
	add_definitions("-DSC_PLUGIN_EXT=\".so\"")
endif()

if(UNIX AND NOT APPLE)
	set(LINUX_SC_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/SuperCollider/plugins)
endif()

include_directories(${CMAKE_SOURCE_DIR}/external_libraries
					${CMAKE_SOURCE_DIR}/external_libraries/nova-simd
					${CMAKE_SOURCE_DIR}/external_libraries/nova-tt
					)

include_directories(${boost_include_dirs})

add_subdirectory(plugins)
add_subdirectory(scsynth)

if (SUPERNOVA)
	add_subdirectory(supernova)
endif()

set(plugins "${plugins}" PARENT_SCOPE)
set(supernova_plugins "${supernova_plugins}" PARENT_SCOPE)
