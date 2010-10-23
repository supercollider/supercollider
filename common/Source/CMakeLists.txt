include_directories(../Headers/common
                    ../Headers/plugin_interface)

if (NOVA_SIMD)
	add_definitions("-DNOVA_SIMD")
	include_directories(../include/nova-simd)
endif()

if(CURL)
	find_package(CURL)
	if(CURL_FOUND)
		add_definitions("-DHAVE_CURL")
		include_directories(${CURL_INCLUDE_DIRS})
	endif()
endif()

add_definitions("-DSC_PLUGIN_LOAD_SYM=\"load\"")

if (NOT APPLE)
	# on apple we leave this undefined so it defaults to .scx (in the code)
	add_definitions("-DSC_PLUGIN_EXT=\".so\"")
endif()

add_subdirectory(common)
add_subdirectory(lang)
add_subdirectory(plugins)
add_subdirectory(server)
