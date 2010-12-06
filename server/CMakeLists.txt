if(WIN32)
    add_definitions("-DSC_PLUGIN_LOAD_SYM=\"_load\"")
else()
    add_definitions("-DSC_PLUGIN_LOAD_SYM=\"load\"")
endif()

if (NOT APPLE AND NOT WIN32)
	# on apple we leave this undefined so it defaults to .scx (in the code)
	add_definitions("-DSC_PLUGIN_EXT=\".so\"")
endif()

add_subdirectory(scsynth)
add_subdirectory(plugins)
