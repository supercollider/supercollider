cmake_minimum_required (VERSION 2.8)
project (SuperCollider)

include(CTest)
enable_testing()

include (cmake_modules/FinalFile.cmake)

if (NOT CMAKE_BUILD_TYPE)
	message(STATUS "Build type defaulting to \"Release\"")
	set(CMAKE_BUILD_TYPE "Release")
endif()

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(WARNING "WARNING: IN-PLACE BUILDS ARE NOT RECOMMENDED - PLEASE USE A BUILD DIRECTORY")
endif()


set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules
                      ${CMAKE_MODULE_PATH})

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

if(NOT WIN32)
	ADD_CUSTOM_TARGET(setMainVersion ALL
		sh ${CMAKE_CURRENT_SOURCE_DIR}/setMainVersion.sh
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		VERBATIM
		)
endif()

if(APPLE)
	STRING(REGEX REPLACE "^.*MacOSX10.([0-9]+).*$" "\\1" MAC_OSX_SDK_VERSION  "${CMAKE_OSX_SYSROOT}")

	if(CMAKE_OSX_DEPLOYMENT_TARGET)
		if (CMAKE_OSX_DEPLOYMENT_TARGET GREATER 10.4)
			set(HID_UTIL_DIR "HID_Utilities")
		else()
			set(HID_UTIL_DIR "HID_Utilities_10_4")
		endif()
	else()
		if (${MAC_OSX_SDK_VERSION} GREATER 4)
			set(HID_UTIL_DIR "HID_Utilities")
		else()
			set(HID_UTIL_DIR "HID_Utilities_10_4")
		endif()
	endif()
	MESSAGE( STATUS "Using HID Utilities from folder '${HID_UTIL_DIR}'")
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(LINUX 1)
endif()

#############################################
# Compiler flags etc

if (${CMAKE_COMPILER_IS_GNUCXX})
	exec_program(${CMAKE_CXX_COMPILER} ARGS -dumpversion OUTPUT_VARIABLE _gcc_version)

	add_definitions("-fschedule-insns2" "-fomit-frame-pointer")

	add_definitions("-Wreturn-type")

#	disabled for now: -ffast-math breaks avx intrinsics and -fsigned-zeros/-fno-associative-math are not available in some versions
#	if (${_gcc_version} VERSION_GREATER 4.0.99)
#		add_definitions("-ffast-math -fsigned-zeros -fno-associative-math")
#	endif()

	option(DSO_VISIBILITY "DSO visibility support for gcc" OFF)

	if (DSO_VISIBILITY)
		add_definitions(-fvisibility=hidden)
	endif()

	if(APPLE)
		exec_program(${CMAKE_CXX_COMPILER} ARGS --version OUTPUT_VARIABLE _gcc_version_info)
		if ("${_gcc_version_info}" MATCHES "Apple")
			add_definitions("-fpascal-strings")
		endif()
		add_definitions("-D_REENTRANT")
	elseif(NOT (WIN32 AND NOT CYGWIN))
		add_definitions("-pthread")
	endif()
endif ()

if (${CMAKE_CXX_COMPILER} MATCHES icpc)
	set(CMAKE_COMPILER_IS_INTEL 1)
	add_definitions(-Wno-unknown-pragmas)
	add_definitions(-simd)
endif()

if (${CMAKE_CXX_COMPILER} MATCHES clang)
	option(DSO_VISIBILITY "DSO visibility support for clang" OFF)

	if (DSO_VISIBILITY)
		add_definitions(-fvisibility=hidden)
	endif()

	set(CMAKE_COMPILER_IS_CLANG 1)
endif()

if(APPLE)
	set_property(DIRECTORY
		APPEND
		PROPERTY COMPILE_DEFINITIONS SC_DARWIN)
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

if (MSVC OR MINGW)
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

option(SSE "Compile with support for SSE instructions.")
option(SSE41 "Compile with support for SSE4.1 instructions.")
option(SSE42 "Compile with support for SSE4.2 instructions.")


option(INSTALL_HELP "Install help docs and examples along with the software" ON)
option(INSTALL_OLD_HELP "Install old and deprecated help files" ON)
option(SC_DOC_RENDER "Pre-render SCDoc documentation. (For putting online, etc)" OFF)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    option(SUPERNOVA "Build with supernova as optional audio synthesis server" ON)
elseif()
    option(SUPERNOVA "Build with supernova as optional audio synthesis server" OFF)
endif()

option(SN_MEMORY_DEBUGGING "Build supernova for memory debugging (disable memory pools).")
option(SC_MEMORY_DEBUGGING "Build sclang&scsynth for memory debugging (disable memory pools).")
option(GC_SANITYCHECK "Enable sanity checks in the sclang garbage collector.")

option(NO_LIBSNDFILE "Disable soundfile functionality. (Not recommended.)" OFF)

option(NO_AVAHI "Disable Avahi support. (Not recommended.)" OFF)

if(NOT WIN32)
	option(SCLANG_SERVER "Build with internal server." ON)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	option(SC_WII "Build sclang with WII support" ON)
endif()

option(SC_QT "Build SuperCollider with Qt GUI" ON)
if (SC_QT)
  message( STATUS "Compiling with Qt GUI" )
endif (SC_QT)

if(WIN32)
	option(SC_PSYCOLLIDER "Build PsyCollider" ON)
else()
	option(SC_PSYCOLLIDER "Build PsyCollider" OFF)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    option(NATIVE "Optimize binary for this architecture (binaries may not run on other machines, requires gcc-4.3 or higher).")
    option(LTO "Use GCC's link time optimizer' (experimental).")
endif()

option(SC_SYMLINK_CLASSLIB "Place a symlink of SCCLassLibrary instead of copying" OFF)
option(SC_OX "Try to compile with support for C++0x (experimental, mainly related to supernova)" OFF)

#############################################
# some preprocessor flags
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
	if (SSE)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse -mfpmath=sse")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -mfpmath=sse")
	endif()
	if (SSE41)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.1")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
	endif()
	if (SSE42)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.2")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2")
	endif()
	if (NATIVE)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
	endif()

	if (SC_OX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	endif()
endif()

if (CMAKE_COMPILER_IS_INTEL AND NOT WIN32)
	if (SSE42)
		add_definitions(-xSSE4.2)
	elseif (SSE41)
		add_definitions(-xSSE4.1)
	elseif (SSE)
		add_definitions(-mia32)
	endif()
	if (SC_OX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	endif()

	# disable warnings
	add_definitions(-diag-disable 170) # pointer points outside of underlying object ... used heavily in sclang
	add_definitions(-diag-disable 279) # controlling expression is constant

endif()

if (SC_MEMORY_DEBUGGING)
	add_definitions(-DDISABLE_MEMORY_POOLS)
endif()

#############################################
# subdirectories
add_subdirectory(external_libraries)
add_subdirectory(server)
add_subdirectory(lang)
add_subdirectory(bindings)
add_subdirectory(platform)
add_subdirectory(editors)

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
	install(FILES README.txt ChangeLog
		DESTINATION ${auxresourcesdir}
	)
	install(FILES COPYING AUTHORS
		DESTINATION ${scappdir}
	)
elseif (WIN32)
	set(auxresourcesdir "SuperCollider")
else()
	set(auxresourcesdir "share/SuperCollider")
endif()

if(NOT SC_QT)
	set(SCCLASSLIB_EXCLUDE_REGEX "QtCollider")
else()
	set(SCCLASSLIB_EXCLUDE_REGEX "IGNOREME")
endif()

if(APPLE)
	#if directory contains symlinks, unlink them.
	if(EXISTS "${auxresourcesdir}/SCClassLibrary")
		file(GLOB classlibraryfolders "${auxresourcesdir}/SCClassLibrary/[^.]*")
		foreach(onedirectory ${classlibraryfolders})
			if(IS_SYMLINK "${onedirectory}")
				install( CODE "EXECUTE_PROCESS(COMMAND unlink ${onedirectory} )" )
			endif()
		endforeach(onedirectory)
	endif()

	if(SC_SYMLINK_CLASSLIB)

		#if there are folders inside SCCLassLibrary abort cmake.
		file(GLOB classlibraryfolders "${auxresourcesdir}/SCClassLibrary/[^.]*")
		foreach(onedirectory ${classlibraryfolders})
			if(NOT IS_SYMLINK "${onedirectory}")
				message(FATAL_ERROR "Symlinking will fail ! SCClassLibrary folder already exists and has directories inside, please delete it first." )
			endif()
		endforeach(onedirectory)

		message(STATUS "Will create symlink of SCClassLibrary to ${auxresourcesdir}/SCClassLibrary" )
		if(NOT EXISTS "${auxresourcesdir}/SCClassLibrary")
			install( CODE "EXECUTE_PROCESS(COMMAND mkdir ${auxresourcesdir}/SCClassLibrary)" )
		endif()
		#symlink the folders inside SCCLassLibrary. Check if QtCollider folder should be symlinked or not.
		file(GLOB classlibraryfolders RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/SCClassLibrary" "SCClassLibrary/[^.]*")
		foreach(onedirectory ${classlibraryfolders})
			if(NOT "${onedirectory}" MATCHES "${SCCLASSLIB_EXCLUDE_REGEX}")
				install( CODE
					"EXECUTE_PROCESS(COMMAND ln -s ${CMAKE_CURRENT_SOURCE_DIR}/SCClassLibrary/${onedirectory} ${auxresourcesdir}/SCClassLibrary/${onedirectory} )" )
			endif()
		endforeach(onedirectory)
	else()
		message(STATUS "Will copy SCCLassLibrary to ${auxresourcesdir}")
		install(DIRECTORY SCClassLibrary
			DESTINATION ${auxresourcesdir}
			REGEX ${SCCLASSLIB_EXCLUDE_REGEX} EXCLUDE
		)
	endif()
else()
	install(DIRECTORY SCClassLibrary
		DESTINATION ${auxresourcesdir}
		REGEX ${SCCLASSLIB_EXCLUDE_REGEX} EXCLUDE
		PATTERN "*~" EXCLUDE
		PATTERN "*#" EXCLUDE
	)
endif()


install(DIRECTORY sounds
	DESTINATION ${auxresourcesdir}
)

if(INSTALL_OLD_HELP)
	install(DIRECTORY Help
		DESTINATION ${auxresourcesdir}
	)
endif()

if(INSTALL_HELP)
	install(DIRECTORY examples HelpSource
		DESTINATION ${auxresourcesdir}
		PATTERN "*~" EXCLUDE
		PATTERN "*#" EXCLUDE
	)
endif()

#############################################
#
# build scdoc help files
#
list(APPEND BUILD_CLASSLIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/SCClassLibrary/Common")
list(APPEND BUILD_CLASSLIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/SCClassLibrary/Platform")
list(APPEND BUILD_CLASSLIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/SCClassLibrary/SCDoc")
list(APPEND BUILD_CLASSLIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/SCClassLibrary/DefaultLibrary")
list(APPEND BUILD_CLASSLIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/SCClassLibrary/JITLib")
list(APPEND BUILD_CLASSLIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/SCClassLibrary/backwards_compatibility")

# this folder has an extension to Platform that disables the loadStartupFiles method
list(APPEND BUILD_CLASSLIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/platform/disable_startup_files")

foreach(arg ${BUILD_CLASSLIBRARIES})
	set(BUILD_CLASSLIBRARYPATH "${BUILD_CLASSLIBRARYPATH}+${arg}\n")
endforeach()

configure_file(build_sclang.cfg.in ${CMAKE_CURRENT_BINARY_DIR}/build_sclang.cfg)

if(SC_DOC_RENDER)
	file(GLOB_RECURSE SCDocSources RELATIVE HelpSource .*[^~])
	file(GLOB_RECURSE SCDocClasses RELATIVE SCClassLibrary/SCDoc *.sc)

	add_custom_target(doc ALL
					COMMAND ${CMAKE_CURRENT_BINARY_DIR}/lang/sclang${CMAKE_EXECUTABLE_SUFFIX}
							-l ${CMAKE_CURRENT_BINARY_DIR}/build_sclang.cfg
							platform/renderAllHelp.scd ${CMAKE_CURRENT_SOURCE_DIR}/HelpSource ${CMAKE_CURRENT_BINARY_DIR}/RenderedHelp
					WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
					DEPENDS sclang ${SCDocSources} ${SCDocClasses})

endif()

set_directory_properties(
	PROPERTIES
	ADDITIONAL_MAKE_CLEAN_FILES "RenderedHelp")

#############################################
#
# testsuite
#
add_subdirectory(testsuite)

#############################################
# win32 install support

if(WIN32)
	install(FILES ${FFTW3F_LIBRARY} ${SNDFILE_LIBRARY} ${PTHREADS_LIBRARY} ${PORTAUDIO_LIBRARIES}
		DESTINATION "SuperCollider"
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

#############################################
# CPack support

set(CPACK_PACKAGE_VERSION_MAJOR 3)
set(CPACK_PACKAGE_VERSION_MINOR 5)
set(CPACK_PACKAGE_VERSION_PATCH dev)
include(CPack)
if(WIN32)
	set(CPACK_MONOLITHIC_INSTALL 1)
endif()

#############################################
# hide advanced variables
mark_as_advanced(AVAHI_LIBRARIES AVAHI_INCLUDE_DIRS AVAHI_INCLUDE_DIR AVAHI_LIBRARY-COMMON AVAHI_LIBRARY-CLIENT)
mark_as_advanced(DL)
mark_as_advanced(EMACS_EXECUTABLE)
mark_as_advanced(FFTW3F_INCLUDE_DIR FFTW3F_LIBRARY)
mark_as_advanced(JACK JACK_INCLUDE_DIR JACK_LIBRARY)
mark_as_advanced(MATH_LIBRARY)
mark_as_advanced(QT_QMAKE_EXECUTABLE)
mark_as_advanced(SNDFILE)

