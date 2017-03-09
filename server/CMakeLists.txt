if (NOT APPLE AND NOT WIN32)
	# on apple we leave this undefined so it defaults to .scx (in the code)
	add_definitions("-DSC_PLUGIN_EXT=\".so\"")
endif()

include_directories(${CMAKE_SOURCE_DIR}/external_libraries
					${CMAKE_SOURCE_DIR}/external_libraries/nova-simd
					${CMAKE_SOURCE_DIR}/external_libraries/nova-tt
					)

if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
else()
	include_directories(${CMAKE_SOURCE_DIR}/external_libraries/boost)
endif()

# here we choose who provides us with the FFT lib
if (APPLE)
	add_definitions("-DSC_FFT_VDSP")
else()
	find_package(FFTW3f 3.3)

	if (NOT FFTW3F_FOUND OR FFT_GREEN)
		message(STATUS "Using green fft")
		set(FFT_GREEN 1)
		add_definitions("-DSC_FFT_GREEN")
	else()
		if(WIN32)
			message(STATUS "Found fftw3f: ${FFTW3F_LIBRARY}")
		else(WIN32)
			message(STATUS "Using fftw3f")
		endif(WIN32)
		add_definitions("-DSC_FFT_FFTW")
	endif()
endif()

add_subdirectory(plugins)
add_subdirectory(scsynth)

if (SUPERNOVA)
	add_subdirectory(supernova)
endif()

set(plugins "${plugins}" PARENT_SCOPE)
set(supernova_plugins "${supernova_plugins}" PARENT_SCOPE)
