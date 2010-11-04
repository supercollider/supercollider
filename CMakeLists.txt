cmake_minimum_required (VERSION 2.6)
project (SuperCollider)

include (common/cmake_modules/FinalFile.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/common/cmake_modules
                      ${CMAKE_MODULE_PATH})

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/common/cmake_modules/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

if(APPLE)
	set_property(DIRECTORY
	APPEND
	PROPERTY COMPILE_DEFINITIONS SC_DARWIN)

	exec_program(${CMAKE_C_COMPILER} ARGS --version OUTPUT_VARIABLE _gcc_version_info)
	if ("${_gcc_version_info}" MATCHES "Apple")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpascal-strings")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpascal-strings")
	endif()

elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set_property(DIRECTORY
	APPEND
	PROPERTY COMPILE_DEFINITIONS SC_LINUX)
endif()

option(NOVA_SIMD "Build with nova-simd support." ON)
option(FINAL_BUILD "Build as single source file." OFF)


if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	option(SC_EL "Build emacs-based IDE." ON)
else()
	option(SC_EL "Build emacs-based IDE." OFF)
endif()

option(SC_ED "Build gedit-based IDE." OFF)
option(SC_VIM "Build vim-based IDE." OFF)

option(SCLANG_SERVER "Build with internal server." ON)

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
