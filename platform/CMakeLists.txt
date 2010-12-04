if(APPLE)
	add_subdirectory(mac)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_subdirectory(linux)
endif()