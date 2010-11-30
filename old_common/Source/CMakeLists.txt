include_directories(../Headers/common)

find_package(Pthreads)

if (NOT PTHREADS_FOUND)
    message(SEND_ERROR "cannot find libpthreads")
endif()
include_directories(${PTHREADS_INCLUDE_DIR})


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

if(WIN32)
    add_definitions("-DSC_PLUGIN_LOAD_SYM=\"_load\"")
else()
    add_definitions("-DSC_PLUGIN_LOAD_SYM=\"load\"")
endif()

if (NOT APPLE AND NOT WIN32)
	# on apple we leave this undefined so it defaults to .scx (in the code)
	add_definitions("-DSC_PLUGIN_EXT=\".so\"")
endif()

add_subdirectory(common)
add_subdirectory(lang)
add_subdirectory(plugins)
add_subdirectory(server)
