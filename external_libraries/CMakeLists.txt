if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/nova-simd/vec.hpp)
    message(FATAL_ERROR "It appears you haven't cloned this project's git submodules:"
			" please run `git submodule update --init --recursive` from the root level of the repository.\n"
			"For advice on setting up submodules, and also on keeping them updated over time, please see"
			" this wiki page, and consider saving this link:"
			" https://github.com/supercollider/supercollider/wiki/git-cheat-sheet#working-with-submodules")
endif()

if(MSVC) # hide warnings from 3rd party libraries
	add_compile_options(
		/wd4018 # signed/unsigned mismatch (hidapi)
		/wd4057 # 'operator' : 'identifier1' indirection to slightly different base types from 'identifier2' (hidapi)
		/wd4127 # conditional expression is constant (nova-simd)
		/wd4146 # unary minus operator applied to unsigned type, result still unsigned (boost)
		/wd4189 # local variable is initialized but not referenced (hidapi)
		/wd4244 # conversion from 'type1' to 'type2', possible loss of data (hidapi, boost, nova-simd, portaudio)
		/wd4245 # 'conversion' : conversion from 'type1' to 'type2', signed/unsigned mismatch (hidapi, tlsf, portaudio)
		/wd4267 # conversion from 'size_t' to 'type', possible loss of data (tlsf, portaudio)
		/wd4311 # 'variable': pointer truncation from 'type' to 'type' (tlsf)
		/wd4389 # 'equality-operator': signed/unsigned mismatch (hidapi
		/wd4456 # declaration of 'identifier' hides previous local declaration (hidapi)
		/wd4457 # declaration of 'identifier' hides function parameter (oscpack)
		/wd4459 # declaration of 'identifier' hides global declaration (boost, nova-simd)
		/wd4505 # unreferenced function with internal linkage has been removed (portaudio)
		/wd4701 # potentially uninitialized local variable used (boost)
		/wd4996 # function, class member, variable, or typedef marked deprecated (oscpack, portmidi)
	)
endif()

set(UTF8PROC_INSTALL OFF CACHE BOOL "Install utf8proc" FORCE)
add_subdirectory(utf8proc)

set_property(TARGET utf8proc PROPERTY FOLDER 3rdparty)

if(NOT SYSTEM_PORTAUDIO)
    message(STATUS "PortAudio: building static library")
    add_subdirectory(portaudio)
endif()

if(NOT SYSTEM_BOOST) # we compile boost ourselves
	message(STATUS "Using bundled boost")
	set(BOOST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/boost)

	# thread
	aux_source_directory(boost/libs/thread/src/ boost_thread_src)

	if(WIN32)
		aux_source_directory(boost/libs/thread/src/win32/ boost_thread_platform)
	elseif(PTHREADS_FOUND)
		aux_source_directory(boost/libs/thread/src/pthread/ boost_thread_platform)
	elseif(EMSCRIPTEN)
		# emscripten contains its own pthreads implementation
	else()
		message(SEND_ERROR "we need to implement boost_thread")
	endif()

	add_library(boost_thread_lib STATIC EXCLUDE_FROM_ALL ${boost_thread_src} ${boost_thread_platform})
	target_include_directories(boost_thread_lib PUBLIC ${BOOST_INCLUDE_DIR})

	if(PTHREADS_FOUND)
		target_include_directories(boost_thread_lib PUBLIC ${PTHREADS_INCLUDE_DIR})
	endif()

	if(WIN32)
		target_compile_definitions(boost_thread_lib PUBLIC BOOST_THREAD_USE_LIB)
		target_compile_definitions(boost_thread_lib PRIVATE BOOST_THREAD_BUILD_LIB)
	endif()

	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		target_compile_options(boost_thread_lib PRIVATE -fPIC)
		target_link_libraries(boost_thread_lib rt)
	endif()

	if(LTO)
    set_property(TARGET boost_thread_lib
			APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

		set_property(TARGET boost_thread_lib
			APPEND PROPERTY LINK_FLAGS "-flto -flto-report")
	endif()

	# program_options
	aux_source_directory(boost/libs/program_options/src/ boost_program_options_src )
	add_library(boost_program_options_lib STATIC EXCLUDE_FROM_ALL ${boost_program_options_src})
	target_include_directories(boost_program_options_lib PUBLIC ${BOOST_INCLUDE_DIR})

	# regex
	aux_source_directory(boost/libs/regex/src boost_regex_src)
	add_library(boost_regex_lib STATIC EXCLUDE_FROM_ALL ${boost_regex_src})
	target_include_directories(boost_regex_lib PUBLIC ${BOOST_INCLUDE_DIR})

	# unit_test_framework
	aux_source_directory(boost/libs/test/src boost_unit_test_framework_src)
	add_library(boost_unit_test_framework_lib STATIC EXCLUDE_FROM_ALL ${boost_unit_test_framework_src})
	target_include_directories(boost_unit_test_framework_lib PUBLIC ${BOOST_INCLUDE_DIR})

	# headers
	add_library(boost_headers INTERFACE)
	target_include_directories(boost_headers INTERFACE ${BOOST_INCLUDE_DIR})

	set_property( TARGET
        boost_regex_lib
        boost_thread_lib
        boost_program_options_lib
        boost_unit_test_framework_lib
        PROPERTY FOLDER 3rdparty
    )

    # add aliases for compliance with system boost
    add_library(Boost::thread ALIAS boost_thread_lib)
    add_library(Boost::program_options ALIAS boost_program_options_lib)
    add_library(Boost::regex ALIAS boost_regex_lib)
    add_library(Boost::unit_test_framework ALIAS boost_unit_test_framework_lib)
    add_library(Boost::headers ALIAS boost_headers)

endif()

# add a custom target for boost_sync
add_library(boost_sync INTERFACE)
target_include_directories(boost_sync INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/boost_sync/include
)
# Boost::headers comes from either system or bundled boost
target_link_libraries(boost_sync INTERFACE Boost::headers)
add_library(Boost::sync ALIAS boost_sync)

# tlsf
add_library(tlsf STATIC EXCLUDE_FROM_ALL "TLSF-2.4.6/src/tlsf.c")
target_compile_definitions( tlsf PRIVATE TLSF_STATISTIC=1 )
target_include_directories( tlsf INTERFACE TLSF-2.4.6/src )
set_property(TARGET tlsf PROPERTY POSITION_INDEPENDENT_CODE TRUE)

#oscpack
add_library(oscpack STATIC EXCLUDE_FROM_ALL "oscpack_build.cpp")
target_link_libraries(oscpack PUBLIC Boost::headers)
target_include_directories(oscpack INTERFACE oscpack_1_1_0 )

if(LTO)
	set_property(TARGET oscpack tlsf
				 APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

	set_property(TARGET oscpack tlsf
				 APPEND PROPERTY LINK_FLAGS "-flto -flto-report")
endif()

# yaml-cpp
if(NOT SYSTEM_YAMLCPP)
  message(STATUS "Using bundled yaml-cpp")

  set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
  set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
  set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

  add_subdirectory(${CMAKE_SOURCE_DIR}/external_libraries/yaml-cpp EXCLUDE_FROM_ALL)
  set_property( TARGET yaml-cpp PROPERTY FOLDER 3rdparty )

endif()


set_property( TARGET oscpack tlsf PROPERTY FOLDER 3rdparty )


##### HID_API #######

if (HID_LIBUSB)
  set( LIBUSB ON )
  set( HIDRAW OFF )
endif()
if (HID_HIDRAW)
  set( HIDRAW ON )
  set( LIBUSB OFF )
endif()
set( EXAMPLE_TEST OFF )
set( EXAMPLE_OSC OFF )

if(SC_HIDAPI)
    add_subdirectory(hidapi)
	set_property(TARGET hidapi_parser PROPERTY FOLDER Libs)
	set_property(TARGET hidapi PROPERTY FOLDER Libs)
    message(STATUS "Building with HID support")
else()
    message(STATUS "HID support disabled")
endif()

if(WIN32)
  add_subdirectory(portmidi)
endif()
