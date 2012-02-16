if(APPLE)
	add_subdirectory(mac)
elseif(WIN32)
    add_subdirectory(windows)
endif()

# NB freebsd currently OK to ride on linux for this subdir. eventually may need separating.
if(CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	add_subdirectory(linux)
endif()
