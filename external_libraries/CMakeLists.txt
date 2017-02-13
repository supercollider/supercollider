if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/nova-simd/vec.hpp)
	message(FATAL_ERROR "nova-simd submodule is missing: please run `git submodule init && git submodule update' from the toplevel of your git working tree")
endif()

if(NOT Boost_FOUND) # we compile boost ourselves
	# boost thread
	aux_source_directory(boost/libs/thread/src/ boost_thread_src )

	if(WIN32)
		aux_source_directory(boost/libs/thread/src/win32/ boost_thread_platform )
	elseif(PTHREADS_FOUND)
		aux_source_directory(boost/libs/thread/src/pthread/ boost_thread_platform )
	else()
		message(SEND_ERROR "we need to implement boost_thread")
	endif()

	aux_source_directory(boost/libs/system/src/ boost_system )
	add_library(boost_system STATIC EXCLUDE_FROM_ALL ${boost_system})
	target_include_directories(boost_system PUBLIC boost)

	aux_source_directory(boost/libs/program_options/src/ boost_program_options )
	add_library(boost_program_options STATIC EXCLUDE_FROM_ALL ${boost_program_options})
	target_include_directories(boost_program_options PUBLIC boost)

	aux_source_directory(boost/libs/filesystem/src boost_filesystem_src)
	add_library(boost_filesystem STATIC EXCLUDE_FROM_ALL ${boost_filesystem_src})
	target_include_directories(boost_filesystem PUBLIC boost)

	if(LTO)
		set_property(TARGET boost_program_options boost_system boost_filesystem
					 APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

		set_property(TARGET boost_program_options boost_system boost_filesystem
					 APPEND PROPERTY LINK_FLAGS "-flto -flto-report")
	endif()

	add_library(boost_thread STATIC EXCLUDE_FROM_ALL ${boost_thread_src} ${boost_thread_platform})
	target_link_libraries(boost_thread boost_system)
#	target_compile_definitions(boost_thread PUBLIC BOOST_THREAD_DONT_PROVIDE_INTERRUPTIONS)
	target_include_directories(boost_thread PUBLIC boost)
	target_compile_definitions(boost_thread PUBLIC BOOST_THREAD_USE_LIB) # for win32

	if(PTHREADS_FOUND)
		target_include_directories(boost_thread PUBLIC ${PTHREADS_INCLUDE_DIR})
	endif()
	if(WIN32)
		target_compile_definitions( boost_thread PRIVATE BOOST_THREAD_BUILD_LIB)
	endif()


	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		target_compile_options(boost_thread PRIVATE -fPIC)
		target_link_libraries(boost_thread rt)
	endif()

	if(LTO)
		set_property(TARGET boost_thread
			APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

		set_property(TARGET boost_thread
			APPEND PROPERTY LINK_FLAGS "-flto -flto-report")
	endif()

	set_property( TARGET boost_thread boost_program_options boost_system boost_filesystem PROPERTY FOLDER 3rdparty )

endif()

# tlsf
add_library(tlsf STATIC "TLSF-2.4.6/src/tlsf.c")
target_compile_definitions( tlsf PRIVATE TLSF_STATISTIC=1 )
target_include_directories( tlsf INTERFACE TLSF-2.4.6/src )
set_property(TARGET tlsf PROPERTY POSITION_INDEPENDENT_CODE TRUE)

#oscpack
add_library(oscpack STATIC EXCLUDE_FROM_ALL "oscpack_build.cpp")
target_include_directories(oscpack PRIVATE boost)
target_include_directories(oscpack INTERFACE oscpack_1_1_0 )


if(LTO)
	set_property(TARGET oscpack tlsf
				 APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

	set_property(TARGET oscpack tlsf
				 APPEND PROPERTY LINK_FLAGS "-flto -flto-report")
endif()

if(NOT YAMLCPP_FOUND)
  message(STATUS "using bundled libyaml-cpp")

  # yaml-cpp
  aux_source_directory(${CMAKE_SOURCE_DIR}/external_libraries/yaml-cpp-0.3.0/src yaml_src)
  CREATE_FINAL_FILE(${CMAKE_CURRENT_BINARY_DIR}/libyamlcpp.cpp ${yaml_src})

  add_library(yaml STATIC ${CMAKE_CURRENT_BINARY_DIR}/libyamlcpp.cpp)
  target_include_directories(yaml PUBLIC ${CMAKE_SOURCE_DIR}/external_libraries/yaml-cpp-0.3.0/include)
  set_property( TARGET yaml PROPERTY FOLDER 3rdparty )

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
