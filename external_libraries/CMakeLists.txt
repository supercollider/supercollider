if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/nova-simd/vec.hpp)
    message(FATAL_ERROR "It appears you haven't cloned this project's git submodules:"
			" please run `git submodule update --init --recursive` from the root level of the repository.\n"
			"For advice on setting up submodules, and also on keeping them updated over time, please see"
			" this wiki page, and consider saving this link:"
			" https://github.com/supercollider/supercollider/wiki/git-cheat-sheet#working-with-submodules")
endif()

add_subdirectory(libgit)
set_property(TARGET libgit2package PROPERTY BUILD_FUZZERS OFF)
set_property(TARGET libgit2package PROPERTY BUILD_EXAMPLES OFF)
set_property(TARGET libgit2package PROPERTY BUILD_CLI OFF)
set_property(TARGET libgit2package PROPERTY BUILD_SHARED_LIBS OFF)
set_property(TARGET libgit2 PROPERTY BUILD_FUZZERS OFF)
set_property(TARGET libgit2 PROPERTY BUILD_EXAMPLES OFF)
set_property(TARGET libgit2 PROPERTY BUILD_CLI OFF)
set_property(TARGET libgit2 PROPERTY BUILD_SHARED_LIBS OFF)


if(NOT SYSTEM_PORTAUDIO)
    message(STATUS "PortAudio: building static library")
    add_subdirectory(portaudio)
endif()

if(NOT SYSTEM_BOOST) # we compile boost ourselves
	message(STATUS "Using bundled boost")
	# boost thread
	aux_source_directory(boost/libs/thread/src/ boost_thread_src )

	if(WIN32)
		aux_source_directory(boost/libs/thread/src/win32/ boost_thread_platform )
	elseif(PTHREADS_FOUND)
		aux_source_directory(boost/libs/thread/src/pthread/ boost_thread_platform )
	else()
		message(SEND_ERROR "we need to implement boost_thread")
	endif()

	aux_source_directory(boost/libs/system/src/ boost_system_src )
	add_library(boost_system_lib STATIC EXCLUDE_FROM_ALL ${boost_system_src})
	target_include_directories(boost_system_lib PUBLIC boost)

	aux_source_directory(boost/libs/program_options/src/ boost_program_options_src )
	add_library(boost_program_options_lib STATIC EXCLUDE_FROM_ALL ${boost_program_options_src})
	target_include_directories(boost_program_options_lib PUBLIC boost)


	aux_source_directory(boost/libs/regex/src boost_regex_src)
	add_library(boost_regex_lib STATIC EXCLUDE_FROM_ALL ${boost_regex_src})
	target_include_directories(boost_regex_lib PUBLIC boost)

	add_library(boost_thread_lib STATIC EXCLUDE_FROM_ALL ${boost_thread_src} ${boost_thread_platform})
	target_link_libraries(boost_thread_lib boost_system_lib)
#	target_compile_definitions(boost_thread PUBLIC BOOST_THREAD_DONT_PROVIDE_INTERRUPTIONS)
	target_include_directories(boost_thread_lib PUBLIC boost)
	target_compile_definitions(boost_thread_lib PUBLIC BOOST_THREAD_USE_LIB) # for win32

	if(PTHREADS_FOUND)
		target_include_directories(boost_thread_lib PUBLIC ${PTHREADS_INCLUDE_DIR})
	endif()
	if(WIN32)
		target_compile_definitions( boost_thread_lib PRIVATE BOOST_THREAD_BUILD_LIB)
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

	set_property( TARGET
        boost_regex_lib
        boost_thread_lib
        boost_program_options_lib
        boost_system_lib
        PROPERTY FOLDER 3rdparty
    )
endif()

# tlsf
add_library(tlsf STATIC EXCLUDE_FROM_ALL "TLSF-2.4.6/src/tlsf.c")
target_compile_definitions( tlsf PRIVATE TLSF_STATISTIC=1 )
target_include_directories( tlsf INTERFACE TLSF-2.4.6/src )
set_property(TARGET tlsf PROPERTY POSITION_INDEPENDENT_CODE TRUE)

#oscpack
add_library(oscpack STATIC EXCLUDE_FROM_ALL "oscpack_build.cpp")
target_include_directories(oscpack PRIVATE ${boost_include_dirs})
target_include_directories(oscpack INTERFACE oscpack_1_1_0 )


if(LTO)
	set_property(TARGET oscpack tlsf
				 APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

	set_property(TARGET oscpack tlsf
				 APPEND PROPERTY LINK_FLAGS "-flto -flto-report")
endif()

if(NOT SYSTEM_YAMLCPP)
  message(STATUS "Using bundled yaml-cpp")

  # yaml-cpp
  set(CMAKE_POLICY_VERSION_MINIMUM 3.10) # workaround for cmake 4
  aux_source_directory(${CMAKE_SOURCE_DIR}/external_libraries/yaml-cpp/src yaml_src)
  CREATE_FINAL_FILE(${CMAKE_CURRENT_BINARY_DIR}/libyamlcpp.cpp ${yaml_src})

  add_library(yaml STATIC EXCLUDE_FROM_ALL ${CMAKE_CURRENT_BINARY_DIR}/libyamlcpp.cpp)
  target_include_directories(yaml PUBLIC ${CMAKE_SOURCE_DIR}/external_libraries/yaml-cpp/include ${boost_include_dirs})
  set_property( TARGET yaml PROPERTY FOLDER 3rdparty )
  set(YAMLCPP_LIBRARY yaml)
  set(YAMLCPP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external_libraries/yaml-cpp/include)

  if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(yaml PRIVATE -Wno-deprecated-declarations)
  endif()

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
    message(STATUS "Building with HID support")
elseif(MINGW AND (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5.3.0))
    message(WARNING "SC is by default built without HID-support if a MinGW/gcc-version below 5.3 is used. Please consult the Readme on how to enable it (easy).")
else()
    message(STATUS "HID support disabled")
endif()

if(WIN32)
  add_library(portmidi portmidi/pm_common/pminternal.h
                       portmidi/pm_common/pmutil.c
                       portmidi/pm_common/pmutil.h
                       portmidi/pm_common/portmidi.c
                       portmidi/pm_common/portmidi.h
                       portmidi/pm_win/pmwin.c
                       portmidi/pm_win/pmwinmm.c
                       portmidi/pm_win/pmwinmm.h
                       portmidi/porttime/porttime.c
                       portmidi/porttime/porttime.h
                       portmidi/porttime/ptwinmm.c)
  target_include_directories(portmidi PUBLIC  portmidi/pm_common portmidi/porttime)
  target_include_directories(portmidi PRIVATE portmidi/pm_win)

  target_link_libraries(portmidi PRIVATE winmm.lib)
  target_compile_definitions(portmidi INTERFACE HAVE_PORTMIDI)
endif()
