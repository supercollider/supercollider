cmake_minimum_required (VERSION 2.6)
project (SuperCollider)

include (common/cmake_modules/FinalFile.cmake)

if (NOT CMAKE_BUILD_TYPE)
	message(STATUS "Build type defaulting to \"Release\"")
	set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/common/cmake_modules
                      ${CMAKE_MODULE_PATH})

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/common/cmake_modules/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

ADD_CUSTOM_TARGET(setMainVersion ALL
       sh ${CMAKE_CURRENT_SOURCE_DIR}/common/setMainVersion.sh
       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/common
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
	option(SC_EL "Build emacs-based IDE." ON)
else()
	option(SC_EL "Build emacs-based IDE." OFF)
endif()
option(SC_ED "Build gedit-based IDE." OFF)
option(SC_VIM "Build vim-based IDE." OFF)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	option(SC_WII "Build sclang with WII support" ON)
endif()

#############################################
# subdirectories
add_subdirectory(common/Source)
add_subdirectory(common/build)

if(APPLE)
	add_subdirectory(mac)
endif()

if(SC_EL)
	add_subdirectory(editors/scel)
endif()

if(SC_ED)
	add_subdirectory(editors/sced)
endif()

if(SC_VIM)
	add_subdirectory(editors/scvim)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(DIRECTORY common/Headers/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/SuperCollider
        PATTERN app* EXCLUDE
    )
endif()


#############################################
# CPack support

set(CPACK_PACKAGE_VERSION_MAJOR 3)
set(CPACK_PACKAGE_VERSION_MINOR 5)
set(CPACK_PACKAGE_VERSION_PATCH dev)
include(CPack)
