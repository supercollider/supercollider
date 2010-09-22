include_directories(../Headers/common
                    ../Headers/plugin_interface)

if (${CMAKE_COMPILER_IS_GNUCXX})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -fschedule-insns2 -fomit-frame-pointer")

	if(APPLE)
	set_property(DIRECTORY
					APPEND
					PROPERTY COMPILE_DEFINITIONS _REENTRANT)
	else
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
	endif()
endif ()

if (NOVA_SIMD)
	set_property(DIRECTORY
					APPEND
					PROPERTY COMPILE_DEFINITIONS NOVA_SIMD)
	include_directories(../include/nova-simd)
endif()

set_property(DIRECTORY
				APPEND
				PROPERTY COMPILE_DEFINITIONS SC_PLUGIN_LOAD_SYM=\"load\")

if (NOT APPLE)
	# on apple we leave this undefined so it defaults to .scx (in the code)
	set_property(DIRECTORY
					APPEND
					PROPERTY COMPILE_DEFINITIONS SC_PLUGIN_EXT=\".so\")
endif()

add_subdirectory(common)
add_subdirectory(lang)
add_subdirectory(plugins)
add_subdirectory(server)
