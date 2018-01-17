project (SuperCollider)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(LINUX 1)
endif()

cmake_minimum_required (VERSION 2.8.12)
set(SUPERNOVA_CMAKE_MINVERSION 3.1)

include("SCVersion.txt")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH}")
message(STATUS "SuperCollider Version: ${PROJECT_VERSION}")
message(STATUS "Building from branch ${GIT_BRANCH}, commit hash is ${GIT_COMMIT_HASH}")
include(CTest)
enable_testing()

include (cmake_modules/FinalFile.cmake)

if(CMAKE_GENERATOR MATCHES "Visual Studio" OR CMAKE_GENERATOR MATCHES "Xcode")
	message(STATUS "Please specify the build configuration in the next step")
else()
	if (NOT CMAKE_BUILD_TYPE)
		message(STATUS "Build type defaulting to \"RelWithDebInfo\"")
		set(CMAKE_BUILD_TYPE "RelWithDebInfo")
	endif()
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

# workaround for using ccache with Xcode generator
# thanks to Craig Scott: https://crascit.com/2016/04/09/using-ccache-with-cmake/
get_property(RULE_LAUNCH_COMPILE GLOBAL PROPERTY RULE_LAUNCH_COMPILE)
if(RULE_LAUNCH_COMPILE AND CMAKE_GENERATOR STREQUAL "Xcode")

    # find ccache
    find_program(CCACHE_PROGRAM ccache)

    message(STATUS "Xcode and ccache detected: using ccache to speed up build process")

    # Set up wrapper scripts
    set(SC_LAUNCH_C_SCRIPT   "${CMAKE_BINARY_DIR}/launch-c")
    set(SC_LAUNCH_CXX_SCRIPT "${CMAKE_BINARY_DIR}/launch-cxx")

    configure_file("cmake_modules/launch-c.in"   launch-c)
    configure_file("cmake_modules/launch-cxx.in" launch-cxx)
    execute_process(COMMAND chmod a+rx "${SC_LAUNCH_C_SCRIPT}" "${SC_LAUNCH_CXX_SCRIPT}")

    # Set Xcode project attributes to route compilation and linking
    # through our scripts
    set(CMAKE_XCODE_ATTRIBUTE_CC         "${SC_LAUNCH_C_SCRIPT}")
    set(CMAKE_XCODE_ATTRIBUTE_CXX        "${SC_LAUNCH_CXX_SCRIPT}")
    set(CMAKE_XCODE_ATTRIBUTE_LD         "${SC_LAUNCH_C_SCRIPT}")
    set(CMAKE_XCODE_ATTRIBUTE_LDPLUSPLUS "${SC_LAUNCH_CXX_SCRIPT}")
endif()

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# identify target architecture, build portaudio before any SC specific
# definitions break the pa-build
if(WIN32)
    SET(SC_WIN_BUNDLE_NAME "SuperCollider" CACHE STRING "Folder name of SC install")

    if(CMAKE_CL_64 OR "$ENV{MSYSTEM}" MATCHES "64" OR CMAKE_C_COMPILER MATCHES "64")
        SET(CMAKE_LIBRARY_ARCHITECTURE "x64" CACHE STRING "Architecture of target system (for 64bit)")
    else()
        SET(CMAKE_LIBRARY_ARCHITECTURE "x86" CACHE STRING "Architecture of target system (for 32bit)")
    endif()

    # installing to default application-location creates an error (a privileges and/or path syntax problem)
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        SET(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/Install" CACHE PATH "Cmake install path" FORCE)
    endif()

    # might work with "SHARED" as well.
    set(PA_LIBRARY_TYPE "STATIC")
    if(NOT MSYS)
      set(PORTAUDIO_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/external_libraries/portaudio_sc_org/include" CACHE PATH "Portaudio include directory" FORCE)
      add_subdirectory("${CMAKE_SOURCE_DIR}/external_libraries/portaudio_sc_org")
    endif(NOT MSYS)
endif(WIN32)

#############################################
# Compiler flags etc

if (${CMAKE_COMPILER_IS_GNUCXX})
	exec_program(${CMAKE_CXX_COMPILER} ARGS -dumpversion OUTPUT_VARIABLE _gcc_version)

	add_definitions("-fschedule-insns2" "-fomit-frame-pointer")

	add_definitions("-Wreturn-type")

	if(${_gcc_version} VERSION_LESS 4.8)
		message(FATAL_ERROR "SuperCollider requires at least gcc-4.8 when compiled with gcc.")
	endif()

	add_definitions("-ffast-math -fsigned-zeros -fno-associative-math")

	if(APPLE)
		exec_program(${CMAKE_CXX_COMPILER} ARGS --version OUTPUT_VARIABLE _gcc_version_info)
		if ("${_gcc_version_info}" MATCHES "Apple")
			add_definitions("-fpascal-strings")
		endif()
		add_definitions("-D_REENTRANT")
	elseif(NOT (WIN32 AND NOT CYGWIN))
		add_definitions("-pthread")
	endif()

elseif(${CMAKE_CXX_COMPILER} MATCHES icpc)
	set(CMAKE_COMPILER_IS_INTEL 1)
	add_definitions(-Wno-unknown-pragmas)
	add_definitions(-simd)

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(CMAKE_COMPILER_IS_CLANG 1)
endif()

if(APPLE)
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/Install" CACHE STRING "Cmake install path")
	endif()
elseif(UNIX)
	add_definitions(-DSC_DATA_DIR="${CMAKE_INSTALL_PREFIX}/share/SuperCollider")
endif()

if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX -D_WIN32_WINNT=0x0600)

    #avoid unnecesary autolink
    add_definitions(-DBOOST_DATE_TIME_NO_LIB -DBOOST_ALL_NO_LIB)

endif()

add_definitions(-DBOOST_CHRONO_HEADER_ONLY -DBOOST_NO_AUTO_PTR)


#############################################
# Detect CCache

find_program(CCacheExectuable ccache)
if( CCacheExectuable )
  # only used with >=cmake-3.4
  set( CMAKE_C_COMPILER_LAUNCHER   "${CCacheExectuable}" )
  set( CMAKE_CXX_COMPILER_LAUNCHER "${CCacheExectuable}" )
endif()

#############################################
# Options
option(NOVA_SIMD "Build with nova-simd support." ON)
option(FINAL_BUILD "Build as single source file." OFF)

option(FFT_GREEN "Use internal 'Green' FFT lib rather than FFTW. (Not recommended.)" OFF)

if(NOT ${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm")
    option(SSE "Compile with support for SSE instructions." ON)
    option(SSE2 "Compile with support for SSE2 instructions." ON)
else() # ARM platforms do not have SSE
    set(SSE OFF)
    set(SSE2 OFF)
endif()

set(AUDIOAPI "default" CACHE STRING "Audio API to use (one of {default,coreaudio,jack,portaudio})")

if (AUDIOAPI STREQUAL jack)
	# here we check for JACK metadata API
	include(CheckIncludeFiles)
	CHECK_INCLUDE_FILES("jack/metadata.h" JACK_USE_METADATA_API)
	if(${JACK_USE_METADATA_API})
		message(STATUS "using JACK metadata API")
	    include_directories(${CMAKE_SOURCE_DIR}/external_libraries/jackey)
	endif()
endif()

option(LIBSCSYNTH "Compile libscsynth as shared library" OFF)

option(INSTALL_HELP "Install help docs and examples along with the software" ON)
option(SC_DOC_RENDER "Pre-render SCDoc documentation. (For putting online, etc)" OFF)
option(SC_QT "Build SuperCollider with Qt GUI features" ON)
option(SC_IDE "Build Qt IDE." ON)
if (SC_QT)
  message( STATUS "Compiling with Qt GUI" )
else()
  if (SC_IDE)
	  message( STATUS "Not compiling SC IDE, because it requires Qt but SC_QT is False" )
    set(SC_IDE False)
  endif()
endif (SC_QT)


option(ENABLE_TESTSUITE "Compile testsuite." ON)

if(CMAKE_SYSTEM_NAME MATCHES "Linux|FreeBSD|DragonFly|OpenBSD|NetBSD" AND NOT CMAKE_VERSION VERSION_LESS SUPERNOVA_CMAKE_MINVERSION)
    option(SUPERNOVA "Build with supernova as optional audio synthesis server" ON)
else()
    option(SUPERNOVA "Build with supernova as optional audio synthesis server" OFF)
endif()

option(SN_MEMORY_DEBUGGING "Build supernova for memory debugging (disable memory pools).")
option(SC_MEMORY_DEBUGGING "Build sclang&scsynth for memory debugging (disable memory pools).")
option(GC_SANITYCHECK "Enable sanity checks in the sclang garbage collector.")

option(NO_LIBSNDFILE "Disable soundfile functionality. (Not recommended.)" OFF)
if(WIN32)
    option(NO_AVAHI "Disable Avahi support. (Not recommended.)" ON)
else(WIN32)
    option(NO_AVAHI "Disable Avahi support. (Not recommended.)" OFF)
endif(WIN32)
option(NO_GPL3 "Disable GPL3 code, for pure-GPL2 situations. (Not recommended.)" OFF)

option(SCLANG_SERVER "Build with internal server." ON)

option(SC_WII "Build sclang with WII support (broken on osx after 10.7)" OFF)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
    option(NATIVE "Optimize binary for this architecture (binaries may not run on other machines).")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    option(LTO "Use GCC's link time optimizer' (experimental).")
endif()

option(SC_SYMLINK_CLASSLIB "Place a symlink of SCCLassLibrary instead of copying" OFF)

option(SYSTEM_BOOST   "Use boost libraries from system" OFF)
option(SYSTEM_YAMLCPP "Use yaml-cpp library from system" OFF)

option(FORTIFY "Define -D_FORTIFY_SOURCE=2 to check for programming errors" OFF)

if(MINGW AND (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5.3.0))
    option(SC_HIDAPI "Build with HID support" OFF)
else()
    option(SC_HIDAPI "Build with HID support" ON)
endif()

#############################################
# some default libraries

if (NOT WIN32)
    find_package(Pthreads)

    if (NOT PTHREADS_FOUND)
        message(SEND_ERROR "cannot find libpthreads")
    endif()
    include_directories(${PTHREADS_INCLUDE_DIR})
endif()

if(SYSTEM_BOOST)
  set(Boost_USE_MULTITHREADED      ON)
  find_package( Boost 1.50.0 COMPONENTS  thread system filesystem program_options regex test_exec_monitor )
endif()

if (Boost_FOUND)
	link_directories(${Boost_LIBRARY_DIRS})
else()
	message(STATUS "building boost libraries manually")
endif()

if(SYSTEM_YAMLCPP)
  find_package(YamlCpp)
else()
  set(YAMLCPP_FOUND OFF)
endif()

#############################################
# some preprocessor flags
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
	if (SSE)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}   -msse")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse")
		if(NOT APPLE AND NOT CMAKE_COMPILER_IS_CLANG)
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpmath=sse")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse")
		endif()
	endif()
	if (SSE2)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse2")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
	endif()
	if (NATIVE)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
	endif()

    if( CMAKE_VERSION VERSION_LESS 3.1 )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -std=c++11")
        if(CMAKE_COMPILER_IS_CLANG)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
            set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -stdlib=libc++")
        endif()
    else()
        set( CMAKE_CXX_STANDARD 11 )

        if(CMAKE_COMPILER_IS_CLANG)
            # workaround
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
            set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -stdlib=libc++")
        endif()
     endif()

	if(FORTIFY)
		add_definitions( -D_FORTIFY_SOURCE=2 )
	endif()

	add_definitions(-fvisibility=hidden)
endif()

if (CMAKE_COMPILER_IS_INTEL AND NOT WIN32)
	if (SSE)
		add_definitions(-mia32)
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

	# disable warnings
	add_definitions(-diag-disable 170) # pointer points outside of underlying object ... used heavily in sclang
	add_definitions(-diag-disable 279) # controlling expression is constant
endif()

if(MSVC)
  foreach(flag CMAKE_C_FLAGS CMAKE_CXX_FLAGS)
    set(${flag} "${${flag}} /wd4018") # signed/unsigned mismatch
    set(${flag} "${${flag}} /wd4102") # unreferenced label
    set(${flag} "${${flag}} /wd4267") # conversion from 'size_t' to 'int', possible loss of data
    set(${flag} "${${flag}} /wd4244") # conversion from '__int64' to 'int', possible loss of data
    set(${flag} "${${flag}} /wd4305") # '=' : truncation from 'double' to 'float'
    set(${flag} "${${flag}} /wd4309") # '=' : truncation of constant value
    set(${flag} "${${flag}} /wd4800") # forcing value to bool 'true' or 'false' (performance warning)
    set(${flag} "${${flag}} /wd4996") # The POSIX name for this item is deprecated.
  endforeach()

# _ENABLE_ATOMIC_ALIGNMENT_FIX prevents the build from breaking when VS2015 update 2 upwards are used
# see http://boost.2283326.n4.nabble.com/lockfree-ENABLE-ATOMIC-ALIGNMENT-FIX-for-VS-2015-Update-2-td4685955.html
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D_ENABLE_ATOMIC_ALIGNMENT_FIX)
endif(MSVC)

if (SC_MEMORY_DEBUGGING)
	add_definitions(-DDISABLE_MEMORY_POOLS)
	add_definitions(-DENABLE_MEMORY_CHECKS)
endif()

if(MINGW AND (${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER 4.8.5) AND (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5.3.0))
# no-strict-aliasing was introduced because of problems with MinGW/GCC 4.9.2
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mstackrealign -fno-strict-aliasing")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mstackrealign -fno-strict-aliasing")
elseif(MINGW)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mstackrealign")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mstackrealign")
endif()

# support for building on Raspberry Pi 1/2/3 and BBB
if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SYSTEM_PROCESSOR STREQUAL "armv7l")
  foreach(flag CMAKE_C_FLAGS CMAKE_CXX_FLAGS)
    set(${flag} "${${flag}} -mfloat-abi=hard")
    set(${flag} "${${flag}} -mfpu=neon")
  endforeach()
elseif(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SYSTEM_PROCESSOR STREQUAL "armv6l")
  foreach(flag CMAKE_C_FLAGS CMAKE_CXX_FLAGS)
    set(${flag} "${${flag}} -march=armv6")
    set(${flag} "${${flag}} -mtune=arm1176jzf-s")
    set(${flag} "${${flag}} -mfloat-abi=hard")
    set(${flag} "${${flag}} -mfpu=vfp")
  endforeach()
endif()

if (NO_GPL3)
	add_definitions(-DNO_GPL3)
endif()

#############################################
# subdirectories
add_subdirectory(external_libraries)

if(NOT YAMLCPP_FOUND)
  set(YAMLCPP_LIBRARY yaml)
endif()


configure_file(common/SC_Version.hpp.in common/SC_Version.hpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/common)

file(GLOB_RECURSE ClassLibrary SCClassLibrary/*sc)
file(GLOB_RECURSE HelpSource   HelpSource/*)
file(GLOB_RECURSE Sounds       sounds/*)
file(GLOB_RECURSE Examples     examples/*)


if(APPLE)
  include(cmake_modules/MacAppFolder.cmake)
  set(ide_name ${scappbundlename})
else()
  set(ide_name scide)
endif()

add_subdirectory(server)
add_subdirectory(lang)
add_subdirectory(platform)
add_subdirectory(editors)

if(UNIX AND NOT APPLE)
    install(DIRECTORY include/common include/plugin_interface include/server include/lang
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/SuperCollider
	FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
    )
    install(FILES SCVersion.txt
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/SuperCollider
    )
endif()

if (WIN32)
  set(auxresourcesdir "${SC_WIN_BUNDLE_NAME}" CACHE STRING "Resources directory")
elseif(APPLE)
	set(auxresourcesdir ${scappbundlename} CACHE STRING "Resources directory")
else()
	set(auxresourcesdir "share/SuperCollider" CACHE STRING "Resources directory")
endif()

set(SCCLASSLIB_EXCLUDE_REGEX "IGNOREME")

if(NOT SC_QT)
	set(SCCLASSLIB_EXCLUDE_REGEX "${SCCLASSLIB_EXCLUDE_REGEX}|QtCollider")
endif()

if(WIN32)
	set(SCCLASSLIB_EXCLUDE_REGEX "${SCCLASSLIB_EXCLUDE_REGEX}")
endif()

if(APPLE)
	if(SC_SYMLINK_CLASSLIB)
		#if there are folders inside SCCLassLibrary abort cmake.
		file(GLOB classlibraryfolders "${CMAKE_INSTALL_PREFIX}/${auxresourcesdir}/SCClassLibrary/[^.]*")
		foreach(onedirectory ${classlibraryfolders})
			message( "${onedirectory}" )
			if(NOT IS_SYMLINK "${onedirectory}")
				message(FATAL_ERROR "Symlinking will fail ! SCClassLibrary folder already exists and has directories inside, please delete it first." )
			endif()
		endforeach(onedirectory)

		message(STATUS "Will create symlink of SCClassLibrary to ${CMAKE_INSTALL_PREFIX}/${auxresourcesdir}/SCClassLibrary" )
		if(NOT EXISTS "{CMAKE_INSTALL_PREFIX}/${auxresourcesdir}/SCClassLibrary")
			install( CODE "EXECUTE_PROCESS(COMMAND mkdir ${CMAKE_INSTALL_PREFIX}/${auxresourcesdir}/SCClassLibrary)" )
		endif()
		#symlink the folders inside SCCLassLibrary. Check if QtCollider folder should be symlinked or not.
		file(GLOB classlibraryfolders RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/SCClassLibrary" "SCClassLibrary/[^.]*")
		foreach(onedirectory ${classlibraryfolders})
			if(NOT "${onedirectory}" MATCHES "${SCCLASSLIB_EXCLUDE_REGEX}")
				install( CODE
					"EXECUTE_PROCESS(COMMAND ln -shF ${CMAKE_CURRENT_SOURCE_DIR}/SCClassLibrary/${onedirectory} ${CMAKE_INSTALL_PREFIX}/${auxresourcesdir}/SCClassLibrary/${onedirectory} )" )
			endif()
		endforeach(onedirectory)
	endif()
else()
	install(DIRECTORY SCClassLibrary
		DESTINATION ${auxresourcesdir}
		REGEX ${SCCLASSLIB_EXCLUDE_REGEX} EXCLUDE
		PATTERN "*~" EXCLUDE
		PATTERN "*#" EXCLUDE
	)
endif()

if(NOT APPLE)
	install(DIRECTORY sounds   DESTINATION ${auxresourcesdir})
	install(DIRECTORY examples DESTINATION ${auxresourcesdir})
elseif(APPLE)
	install(DIRECTORY examples DESTINATION ${auxresourcesdir})
endif()

if (WIN32)
	set( AdditionalInstallFiles
		README.md
		README_WINDOWS.md
		CHANGELOG.md
		COPYING
		AUTHORS
	)
elseif(APPLE)
	set( AdditionalInstallFiles
		README.md
		README_MACOS.md
		CHANGELOG.md
		COPYING
		AUTHORS
	)
else()
	set( AdditionalInstallFiles
		README.md
		README_LINUX.md
		CHANGELOG.md
		COPYING
		AUTHORS
	)
endif()

install(FILES ${AdditionalInstallFiles} DESTINATION ${auxresourcesdir})

if(INSTALL_HELP)
	if(SC_SYMLINK_CLASSLIB AND APPLE)
		message(STATUS "Will create symlink of HelpSource to ${CMAKE_INSTALL_PREFIX}/${auxresourcesdir}/HelpSource")
		if(EXISTS "${CMAKE_INSTALL_PREFIX}/${auxresourcesdir}/HelpSource")
			message(FATAL_ERROR "Symlinking will fail ! HelpSource folder already exists, please delete it first." )
		endif()
		install( CODE
					"EXECUTE_PROCESS(COMMAND ln -shF ${CMAKE_CURRENT_SOURCE_DIR}/HelpSource ${CMAKE_INSTALL_PREFIX}/${auxresourcesdir}/HelpSource )" )
	elseif(NOT APPLE)
		install(DIRECTORY HelpSource
			DESTINATION ${auxresourcesdir}
			REGEX ${SCCLASSLIB_EXCLUDE_REGEX} EXCLUDE
			PATTERN "*~" EXCLUDE
			PATTERN "*#" EXCLUDE
		)
	endif()
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
		COMMAND $<TARGET_FILE_DIR:sclang>/sclang${CMAKE_EXECUTABLE_SUFFIX}
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
if(ENABLE_TESTSUITE)
  add_subdirectory(testsuite)
endif()

#############################################
# CPack support

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

if(APPLE)
	set(CPACK_GENERATOR "DragNDrop")
	set(CPACK_DMG_FORMAT "UDBZ")
	set(CPACK_DMG_VOLUME_NAME "${scappbundlename}")
	set(CPACK_SYSTEM_NAME "OSX")
#	set(CPACK_PACKAGE_FILE_NAME "${scappbundlename}-${PROJECT_VERSION}")
	set(CPACK_DMG_DS_STORE "${CMAKE_SOURCE_DIR}/package/ds_store")
	set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/package/background.png")
    set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/icons/SCcube.icns")
endif()

if(NOT WIN32) # We define a custom 'package' target in platform/windows/CMakeLists.txt
    include(CPack)
endif()

#############################################
# hide advanced variables
mark_as_advanced(AVAHI_LIBRARIES AVAHI_INCLUDE_DIRS AVAHI_INCLUDE_DIR AVAHI_LIBRARY-COMMON AVAHI_LIBRARY-CLIENT)
mark_as_advanced(DL)
mark_as_advanced(EMACS_EXECUTABLE)
mark_as_advanced(FFTW3F_INCLUDE_DIR FFTW3F_LIBRARY FFTW3F_LIBRARY_DIR)
mark_as_advanced(JACK JACK_INCLUDE_DIR JACK_LIBRARY)
mark_as_advanced(MATH_LIBRARY)
mark_as_advanced(QT_QMAKE_EXECUTABLE)
mark_as_advanced(SNDFILE SNDFILE_LIBRARY_DIR)
mark_as_advanced(SC_HIDAPI)
