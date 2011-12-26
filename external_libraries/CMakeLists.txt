if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/nova-simd/vec.hpp)
    message(FATAL_ERROR "nova-simd submodule is missing: please run `git submodule init && git submodule update' from the toplevel of your git working tree")
endif()

# boost thread
aux_source_directory(boost/libs/thread/src/ boost_thread_src )

include_directories(boost)

if(WIN32)
	aux_source_directory(boost/libs/thread/src/win32/ boost_thread_platform )
	add_definitions(-DBOOST_THREAD_BUILD_DLL)
elseif(PTHREADS_FOUND)
	aux_source_directory(boost/libs/thread/src/pthread/ boost_thread_platform )
else()
	message(SEND_ERROR "we need to implement boost_thread")
endif()

include_directories(${PTHREADS_INCLUDE_DIR})

if (SUPERNOVA)
	aux_source_directory(boost/libs/program_options/src/ boost_program_options )
	add_library(boost_program_options STATIC ${boost_program_options})

	aux_source_directory(boost/libs/system/src/ boost_system )
	add_library(boost_system STATIC ${boost_system})

	aux_source_directory(boost/libs/filesystem/v3/src boost_filesystem_v3)
	add_library(boost_filesystem STATIC ${boost_filesystem_v2} ${boost_filesystem_v3})


	#oscpack
	add_library(oscpack STATIC "oscpack_build.cpp")

	#tlsf
	add_library(tlsf STATIC "TLSF-2.4.6/src/tlsf.c")
	set_target_properties(tlsf
						PROPERTIES COMPILE_DEFINITIONS TLSF_STATISTIC=1)


	if(LTO)
		set_property(TARGET boost_program_options boost_system boost_filesystem oscpack tlsf
					APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

		set_property(TARGET boost_program_options boost_system boost_filesystem oscpack tlsf
					APPEND PROPERTY LINK_FLAGS "-flto -flto-report")
	endif()
endif()

if (${DSO_VISIBILITY})
	remove_definitions(-fvisibility=hidden)
endif()

add_library(boost_thread STATIC ${boost_thread_src} ${boost_thread_platform})

add_library(supercollider_boost_thread SHARED ${boost_thread_src} ${boost_thread_platform})
target_link_libraries(supercollider_boost_thread ${PTHREADS_LIBRARY})

if(LTO)
	set_property(TARGET boost_thread supercollider_boost_thread
				APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

	set_property(TARGET boost_thread supercollider_boost_thread
				APPEND PROPERTY LINK_FLAGS "-flto -flto-report")
endif()


if(APPLE)
	# determines the app name and app install location (scappbundlename, scappdir):
	include (${CMAKE_SOURCE_DIR}/cmake_modules/MacAppFolder.cmake)

	install(TARGETS supercollider_boost_thread
			DESTINATION "${scappauxresourcesdir}"
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
			COMPONENT app)
elseif(WIN32)
	install(TARGETS supercollider_boost_thread
			DESTINATION "SuperCollider"
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
else()
	install(TARGETS supercollider_boost_thread
			LIBRARY DESTINATION "lib"
			RUNTIME DESTINATION "bin"
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()
