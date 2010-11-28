if(APPLE)
	add_subdirectory(mac)
	add_subdirectory(osx)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_subdirectory(linux)
endif()