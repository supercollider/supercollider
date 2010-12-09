cmake_minimum_required (VERSION 2.6)
project (SuperCollider)

include (cmake_modules/FinalFile.cmake)

if (NOT CMAKE_BUILD_TYPE)
	message(STATUS "Build type defaulting to \"Release\"")
	set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules
                      ${CMAKE_MODULE_PATH})

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

ADD_CUSTOM_TARGET(setMainVersion ALL
       sh ${CMAKE_CURRENT_SOURCE_DIR}/setMainVersion.sh
       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
       VERBATIM
      )

#############################################
# Compiler flags etc

if (${CMAKE_COMPILER_IS_GNUCXX})
	add_definitions("-ffast-math" "-fschedule-insns2" "-fomit-frame-pointer")

	if(APPLE)
		add_definitions("-D_REENTRANT")
	else()
		add_definitions("-pthread")
	endif()
endif ()

if(APPLE)
	set_property(DIRECTORY
		APPEND
		PROPERTY COMPILE_DEFINITIONS SC_DARWIN)
	exec_program(${CMAKE_C_COMPILER} ARGS --version OUTPUT_VARIABLE _gcc_version_info)
	if ("${_gcc_version_info}" MATCHES "Apple")
		add_definitions("-fpascal-strings")
	endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_definitions("-DSC_LINUX")
	add_definitions(-DSC_DATA_DIR="${CMAKE_INSTALL_PREFIX}/share/SuperCollider")
endif()

if(WIN32)
	add_definitions("-DSC_WIN32")
endif()


#############################################
# some default libraries

find_package(Pthreads)

if (NOT PTHREADS_FOUND)
    message(SEND_ERROR "cannot find libpthreads")
endif()
include_directories(${PTHREADS_INCLUDE_DIR})



if(CURL)
	find_package(CURL)
	if(CURL_FOUND)
		add_definitions("-DHAVE_LIBCURL")
		include_directories(${CURL_INCLUDE_DIRS})
	endif()
endif()

#############################################
# Options
option(NOVA_SIMD "Build with nova-simd support." ON)
option(FINAL_BUILD "Build as single source file." OFF)

option(CURL "Build with curl for remote file download." ON)

option(FFT_GREEN "Use internal 'Green' FFT lib rather than FFTW. (Not recommended.)" OFF)
mark_as_advanced(FFT_GREEN)

option(NO_LIBSNDFILE "Disable soundfile functionality. (Not recommended.)" OFF)
mark_as_advanced(NO_LIBSNDFILE)

if(NOT WIN32)
	option(SCLANG_SERVER "Build with internal server." ON)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	option(SC_WII "Build sclang with WII support" ON)
endif()

#############################################
# subdirectories
add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(lang)
add_subdirectory(bindings)
add_subdirectory(platform)
add_subdirectory(editors)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(DIRECTORY include
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/SuperCollider
        PATTERN app* EXCLUDE
    )
endif()

if(APPLE)
	# determines the app name and app install location (scappbundlename, scappdir):
	include (cmake_modules/MacAppFolder.cmake)

	set(auxresourcesdir ${scappauxresourcesdir})

	# Some aux files which go in the app's folder
	install(FILES README ChangeLog
		DESTINATION ${auxresourcesdir}
	)
	install(FILES COPYING AUTHORS
		DESTINATION ${scappdir}
	)
else()
	set(auxresourcesdir "share/SuperCollider")
endif()

install(DIRECTORY SCClassLibrary
	DESTINATION ${auxresourcesdir}
	PATTERN .git EXCLUDE
)

install(DIRECTORY Help
	DESTINATION ${auxresourcesdir}
	PATTERN .git EXCLUDE
)

install(DIRECTORY examples
	DESTINATION ${auxresourcesdir}
	PATTERN .git EXCLUDE
)

install(DIRECTORY sounds
	DESTINATION ${auxresourcesdir}
	PATTERN .git EXCLUDE
)

#############################################
# CPack support

set(CPACK_PACKAGE_VERSION_MAJOR 3)
set(CPACK_PACKAGE_VERSION_MINOR 5)
set(CPACK_PACKAGE_VERSION_PATCH dev)
include(CPack)
