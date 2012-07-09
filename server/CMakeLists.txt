if (NOT APPLE AND NOT WIN32)
	# on apple we leave this undefined so it defaults to .scx (in the code)
	add_definitions("-DSC_PLUGIN_EXT=\".so\"")
endif()

include_directories(${CMAKE_SOURCE_DIR}/external_libraries
					${CMAKE_SOURCE_DIR}/external_libraries/boost-lockfree
					${CMAKE_SOURCE_DIR}/external_libraries/nova-simd
					${CMAKE_SOURCE_DIR}/external_libraries/nova-tt
					)


if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
else()
	include_directories(${CMAKE_SOURCE_DIR}/external_libraries/boost)
endif()


add_subdirectory(plugins)
add_subdirectory(scsynth)

if (SUPERNOVA)
	add_subdirectory(supernova)
endif()
