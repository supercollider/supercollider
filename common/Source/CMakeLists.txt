include_directories(../Headers/common
                    ../Headers/plugin_interface)

if (${CMAKE_COMPILER_IS_GNUCXX})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -fschedule-insns2 -fomit-frame-pointer")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif ()


set_property(DIRECTORY
				APPEND
				PROPERTY COMPILE_DEFINITIONS NOVA_SIMD)
include_directories(../include/nova-simd)

set_property(DIRECTORY
				APPEND
				PROPERTY COMPILE_DEFINITIONS SC_PLUGIN_LOAD_SYM=\"load\")

set_property(DIRECTORY
				APPEND
				PROPERTY COMPILE_DEFINITIONS SC_PLUGIN_EXT=\".so\")

add_subdirectory(common)
add_subdirectory(lang)
add_subdirectory(plugins)
add_subdirectory(server)
