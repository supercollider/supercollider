include_directories(../Headers/common
                    ../Headers/plugin_interface)

if (NOVA_SIMD)
	add_definitions("-DNOVA_SIMD")
	include_directories(../include/nova-simd)
endif()

if(CURL)
	find_package(Curl)
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
