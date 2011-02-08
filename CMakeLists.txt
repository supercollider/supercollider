cmake_minimum_required (VERSION 2.6)
project (SuperCollider)
enable_testing()

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

if(APPLE)
	STRING(REGEX REPLACE "^.*MacOSX10.([0-9]+).*$" "\\1" MAC_OSX_SDK_VERSION  "${CMAKE_OSX_SYSROOT}")

	if(MAC_OSX_SDK_VERSION GREATER 4)
		set(HID_UTIL_DIR "HID_Utilities")
	else()
		set(HID_UTIL_DIR "HID_Utilities_10_4")
	endif()
	MESSAGE( STATUS "Using HID Utilities from folder '${HID_UTIL_DIR}'")
endif()

#############################################
# Compiler flags etc

if (${CMAKE_COMPILER_IS_GNUCXX})
	add_definitions("-ffast-math" "-fschedule-insns2" "-fomit-frame-pointer")

	option(DSO_VISIBILITY "DSO visibility support for gcc" OFF)

	if (DSO_VISIBILITY)
		add_definitions(-fvisibility=hidden)
	endif()

	if(APPLE)
		add_definitions("-D_REENTRANT")
	elseif(NOT (WIN32 AND NOT CYGWIN))
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
elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	add_definitions("-DSC_FREEBSD")
	add_definitions(-DSC_DATA_DIR="${CMAKE_INSTALL_PREFIX}/share/SuperCollider")
endif()

if(WIN32)
    set_property(DIRECTORY
                 APPEND
                 PROPERTY COMPILE_DEFINITIONS SC_WIN32 NOMINMAX _WINSOCKAPI_)

    if(MSVC)
        SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} /MTd")
        SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} /MT")
    endif()
endif()


#############################################
# some default libraries

find_package(Pthreads)

if (NOT PTHREADS_FOUND)
    message(SEND_ERROR "cannot find libpthreads")
endif()
include_directories(${PTHREADS_INCLUDE_DIR})

if (MSVC)
    set(MATH_LIBRARY "")
else()
    find_library(MATH_LIBRARY m)
endif()

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

option(SSE "Compile with support for SSE instructions.")
option(SSE41 "Compile with support for SSE4.1 instructions.")
option(SSE42 "Compile with support for SSE4.2 instructions.")


if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    option(SUPERNOVA "Build with supernova as optional audio synthesis server" ON)
elseif()
    option(SUPERNOVA "Build with supernova as optional audio synthesis server" OFF)
endif()

option(SN_MEMORY_DEBUGGING "Build supernova for memory debugging (disable memory pools).")
option(SC_MEMORY_DEBUGGING "Build sclang&scsynth for memory debugging (disable memory pools).")
option(GC_SANITYCHECK "Enable sanity checks in the sclang garbage collector.")

option(NO_LIBSNDFILE "Disable soundfile functionality. (Not recommended.)" OFF)
mark_as_advanced(NO_LIBSNDFILE)

if(NOT WIN32)
	option(SCLANG_SERVER "Build with internal server." ON)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	option(SC_WII "Build sclang with WII support" ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    option(NATIVE "Optimize binary for this architecture (binaries may not run on other machines, requires gcc-4.3 or higher).")
    option(LTO "Use GCC's link time optimizer' (experimental).")
endif()

#############################################
# some preprocessor flags
if(CMAKE_COMPILER_IS_GNUCXX)
  if(SSE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse -mfpmath=sse")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -mfpmath=sse")
  endif()
  if(SSE41)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
  endif()
  if(SSE42)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2")
  endif()
  if(NATIVE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
  endif()
endif()

if (SC_MEMORY_DEBUGGING)
	add_definitions(-DDISABLE_MEMORY_POOLS)
endif()

#############################################
# subdirectories
add_subdirectory(server)
add_subdirectory(lang)
add_subdirectory(bindings)
add_subdirectory(platform)
add_subdirectory(editors)
add_subdirectory(external_libraries)
add_subdirectory(testsuite)

if(CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    install(DIRECTORY include/common include/plugin_interface include/server include/lang
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/SuperCollider
        FILES_MATCHING PATTERN "*.h"
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
