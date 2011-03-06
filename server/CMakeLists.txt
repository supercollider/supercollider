if (NOT APPLE AND NOT WIN32)
	# on apple we leave this undefined so it defaults to .scx (in the code)
	add_definitions("-DSC_PLUGIN_EXT=\".so\"")
endif()


include_directories(${CMAKE_SOURCE_DIR}/external_libraries
                    ${CMAKE_SOURCE_DIR}/external_libraries/boost
                    ${CMAKE_SOURCE_DIR}/external_libraries/boost_lockfree
                    ${CMAKE_SOURCE_DIR}/external_libraries/nova-simd
                    ${CMAKE_SOURCE_DIR}/external_libraries/nova-tt
                    )

add_subdirectory(plugins)
add_subdirectory(scsynth)

if (SUPERNOVA)
  add_subdirectory(supernova)
endif()
