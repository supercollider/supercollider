# first check cmake configs (e.g. from vcpkg), the fall back to pkg-config
find_package(portaudio CONFIG QUIET)

if(TARGET PortAudio::PortAudio)
	# we already got the correct target name
elseif(TARGET portaudio)
	add_library(PortAudio::PortAudio ALIAS portaudio)
elseif(TARGET portaudio::portaudio)
	add_library(PortAudio::PortAudio ALIAS portaudio::portaudio)
elseif(TARGET unofficial::portaudio::portaudio)
	add_library(PortAudio::PortAudio ALIAS unofficial::portaudio::portaudio)
else()
	# this should only be run on systems with pkg-config installed
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(PORTAUDIO_SYSTEM_LIB REQUIRED IMPORTED_TARGET portaudio-2.0)

	# alias for using a consistent name throughout the codebase
	add_library(PortAudio::PortAudio ALIAS PkgConfig::PORTAUDIO_SYSTEM_LIB)
endif()
