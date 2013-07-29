if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/nova-simd/vec.hpp)
	message(FATAL_ERROR "nova-simd submodule is missing: please run `git submodule init && git submodule update' from the toplevel of your git working tree")
endif()

if(NOT Boost_FOUND) # we compile boost ourselves
	# boost thread
	aux_source_directory(boost/libs/thread/src/ boost_thread_src )

	include_directories(boost)

	if(WIN32)
		aux_source_directory(boost/libs/thread/src/win32/ boost_thread_platform )
		add_definitions(-DBOOST_THREAD_BUILD_LIB)
	elseif(PTHREADS_FOUND)
		include_directories(${PTHREADS_INCLUDE_DIR})
		aux_source_directory(boost/libs/thread/src/pthread/ boost_thread_platform )
	else()
		message(SEND_ERROR "we need to implement boost_thread")
	endif()

	aux_source_directory(boost/libs/system/src/ boost_system )
	add_library(boost_system STATIC ${boost_system})

	if (SUPERNOVA)
		aux_source_directory(boost/libs/program_options/src/ boost_program_options )
		add_library(boost_program_options STATIC ${boost_program_options})

		aux_source_directory(boost/libs/filesystem/src boost_filesystem_src)
		add_library(boost_filesystem STATIC ${boost_filesystem_src})

		if(LTO)
			set_property(TARGET boost_program_options boost_system boost_filesystem
						 APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

			set_property(TARGET boost_program_options boost_system boost_filesystem
						 APPEND PROPERTY LINK_FLAGS "-flto -flto-report")
		endif()
	endif()

	# still required by threadpool
	add_library(boost_thread STATIC ${boost_thread_src} ${boost_thread_platform})
	target_link_libraries(boost_thread boost_system)

	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		add_definitions(-fPIC)
		target_link_libraries(boost_thread rt)
	endif()

	if(LTO)
		set_property(TARGET boost_thread
			APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

		set_property(TARGET boost_thread
			  APPEND PROPERTY LINK_FLAGS "-flto -flto-report")
	endif()
endif()

if (SUPERNOVA)
	#oscpack
	add_library(oscpack STATIC "oscpack_build.cpp")

	#tlsf
	add_library(tlsf STATIC "TLSF-2.4.6/src/tlsf.c")
	set_target_properties(tlsf
						 PROPERTIES COMPILE_DEFINITIONS TLSF_STATISTIC=1)
	if(LTO)
		set_property(TARGET oscpack tlsf
					 APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

		set_property(TARGET oscpack tlsf
					 APPEND PROPERTY LINK_FLAGS "-flto -flto-report")
	endif()
endif()

if(NOT YAMLCPP_FOUND)
  message(STATUS "using bundled libyaml-cpp")

  # yaml-cpp
  include_directories(${CMAKE_SOURCE_DIR}/external_libraries/yaml-cpp-0.3.0/include)
  aux_source_directory(${CMAKE_SOURCE_DIR}/external_libraries/yaml-cpp-0.3.0/src yaml_src)
  CREATE_FINAL_FILE(${CMAKE_CURRENT_BINARY_DIR}/libyamlcpp.cpp ${yaml_src})

  add_library(yaml STATIC ${CMAKE_CURRENT_BINARY_DIR}/libyamlcpp.cpp)
endif()

# portaudio
if (WIN32 OR APPLE)
    include_directories(${CMAKE_SOURCE_DIR}/external_libraries/portaudio/include)
    include_directories(${CMAKE_SOURCE_DIR}/external_libraries/portaudio/src/common)

    aux_source_directory( portaudio/src/common pa_common )

    if (APPLE)
        aux_source_directory( portaudio/src/os/unix pa_platform )
        aux_source_directory( portaudio/src/hostapi/coreaudio pa_hostapi )
    elseif(WIN32)
        if(NOT ASIOSDK_ROOT_DIR)
            message(FATAL_ERROR "Please set ASIOSDK_ROOT_DIR!")
        endif()
        aux_source_directory( portaudio/src/os/win pa_platform )
        aux_source_directory( portaudio/src/hostapi/wmme pa_wmme )
        set( pa_asio
            portaudio/src/hostapi/asio/pa_asio.cpp
            ${ASIOSDK_ROOT_DIR}/common/asio.cpp
            ${ASIOSDK_ROOT_DIR}/host/pc/asiolist.cpp
            ${ASIOSDK_ROOT_DIR}/host/asiodrivers.cpp
        )
        set( pa_hostapi ${pa_wmme} ${pa_asio} )
        include_directories(${ASIOSDK_ROOT_DIR}/common)
        include_directories(${ASIOSDK_ROOT_DIR}/host)
        include_directories(${ASIOSDK_ROOT_DIR}/host/pc)
        include_directories(portaudio/src/os/win)

        set_property(SOURCE ${pa_asio} PROPERTY COMPILE_DEFINITIONS "interface=__interface")

    endif()

    add_library(portaudio STATIC ${pa_common} ${pa_platform} ${pa_hostapi})

    if (WIN32)
        set_target_properties(portaudio PROPERTIES COMPILE_DEFINITIONS "PA_USE_WMME=1;PA_USE_ASIO=1;_UNICODE")
    endif()

    if (APPLE)
        set_target_properties(portaudio
                              PROPERTIES COMPILE_DEFINITIONS PA_USE_COREAUDIO)
        target_link_libraries(portaudio "-framework CoreAudio -framework AudioUnit -framework AudioToolbox")
    endif()

endif()
