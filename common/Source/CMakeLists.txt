include_directories(../Headers/common
                    ../Headers/plugin_interface)

if (${CMAKE_COMPILER_IS_GNUCXX})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -fschedule-insns2 -fomit-frame-pointer")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
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

set_property(DIRECTORY
				APPEND
				PROPERTY COMPILE_DEFINITIONS SC_PLUGIN_EXT=\".so\")

find_package (Qt4)
if (QT4_FOUND)
  option( SC_QT "Compile SuperCollider with Qt GUI?" YES )
else (QT4_FOUND)
  option( SC_QT "Compile SuperCollider with Qt GUI?" NO )
endif (QT4_FOUND)

if (SC_QT)
  message( STATUS "Compiling with Qt GUI" )
  add_subdirectory(QtCollider)
endif (SC_QT)

add_subdirectory(common)
add_subdirectory(lang)
add_subdirectory(plugins)
add_subdirectory(server)
