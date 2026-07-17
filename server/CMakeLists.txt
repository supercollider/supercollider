if(UNIX AND NOT APPLE)
	set(LIB_SUFFIX "" CACHE STRING "Install plugins to suffixed lib directory: eg. `64` for `lib64`")
	set(LINUX_SC_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/SuperCollider/plugins)
endif()

include_directories(${CMAKE_SOURCE_DIR}/external_libraries
					${CMAKE_SOURCE_DIR}/external_libraries/nova-simd
					${CMAKE_SOURCE_DIR}/external_libraries/nova-tt
					)


add_subdirectory(plugins)
add_subdirectory(scsynth)

if (SUPERNOVA)
	add_subdirectory(supernova)
endif()

set(plugins "${plugins}" PARENT_SCOPE)
set(supernova_plugins "${supernova_plugins}" PARENT_SCOPE)
