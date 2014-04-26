if(WIN32)
    add_subdirectory(windows)
endif()

# NB bsd currently OK to ride on linux for this subdir. eventually may need separating.
if(UNIX AND NOT APPLE)
	add_subdirectory(linux)
endif()
