if(WIN32)
	remove_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build PortMidi as a static lib" FORCE)
add_subdirectory(portmidi_submodule)
target_include_directories(portmidi PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/portmidi_submodule/porttime> # add missing include
)
