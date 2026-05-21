cmake_minimum_required(VERSION 3.12)

# pick up vcpkg installation, if VCPKG_ROOT is set
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE AND NOT "$ENV{VCPKG_ROOT}" STREQUAL "")
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    message(STATUS "Using CMAKE_TOOLCHAIN_FILE at ${CMAKE_TOOLCHAIN_FILE}")
endif()

include(SCVersion.txt)
# CMake versions can only consist of digits and periods, so don't use _TWEAK
project(SuperCollider VERSION ${SC_VERSION_MAJOR}.${SC_VERSION_MINOR}.${SC_VERSION_PATCH})

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(LINUX 1)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux|FreeBSD|DragonFly|OpenBSD|NetBSD")
    set(LINUX_OR_BSD 1)
endif()

message(STATUS "SuperCollider Version: ${SC_VERSION}")
message(STATUS "Building from ${GIT_REF_TYPE} ${GIT_BRANCH_OR_TAG}, commit hash is ${GIT_COMMIT_HASH}")

# Stops CTest from generating a bunch of targets we don't care about.
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
include(CTest)
enable_testing()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

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

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
set_property(TARGET uninstall PROPERTY FOLDER Util)

# identify target architecture
if(WIN32)
    SET(SC_WIN_BUNDLE_NAME "SuperCollider" CACHE STRING "Folder name of SC install")

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        SET(CMAKE_LIBRARY_ARCHITECTURE "x64" CACHE STRING "Architecture of target system (for 64bit)")
    else()
        SET(CMAKE_LIBRARY_ARCHITECTURE "x86" CACHE STRING "Architecture of target system (for 32bit)")
    endif()

endif(WIN32)

# install settings
if(APPLE OR WIN32)
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/Install" CACHE PATH "Cmake install path" FORCE)
	endif()
elseif(UNIX)
	add_compile_definitions(SC_DATA_DIR="${CMAKE_INSTALL_PREFIX}/share/SuperCollider")
endif()

#############################################
# Options
option(NOVA_SIMD "Build with nova-simd support." ON)
option(FINAL_BUILD "Build as single source file." OFF)
option(USE_CCACHE "Use ccache if available." ON)

option(FFT_GREEN "Use internal 'Green' FFT lib rather than FFTW. (Not recommended.)" OFF)

set(AUDIOAPI "default" CACHE STRING "Audio API to use (one of {default,coreaudio,jack,portaudio,bela})")

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
option(NO_X11 "Disables the building of plugins that require the X11 library (for UI-less compiles)" OFF)

option(ENABLE_TESTSUITE "Compile testsuite." ON)

option(SUPERNOVA "Build with supernova as an alternative audio synthesis server" ON)

option(SN_MEMORY_DEBUGGING "Build supernova for memory debugging (disable memory pools).")
option(SC_MEMORY_DEBUGGING "Build sclang&scsynth for memory debugging (disable memory pools).")
option(GC_SANITYCHECK "Enable sanity checks in the sclang garbage collector.")

option(NO_LIBSNDFILE "Disable soundfile functionality. (Not recommended.)" OFF)
if(WIN32)
    option(NO_AVAHI "Disable Avahi support. (Not recommended.)" ON)
else(WIN32)
    option(NO_AVAHI "Disable Avahi support. (Not recommended.)" OFF)
endif(WIN32)

option(SCLANG_SERVER "Build with internal server." ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    option(NATIVE "Optimize binary for this architecture (binaries may not run on other machines).")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    option(LTO "Use GCC's link time optimizer' (experimental).")
endif()

option(SC_SYMLINK_CLASSLIB "Place a symlink of SCCLassLibrary instead of copying" OFF)

if(LINUX)
    option(SC_CLANG_USES_LIBSTDCPP "Use libstdc++ with clang instead of libc++" ON)
else()
    option(SC_CLANG_USES_LIBSTDCPP "Use libstdc++ with clang instead of libc++" OFF)
endif()

option(SYSTEM_ABLETON_LINK "Use link from system" OFF)
option(SYSTEM_BOOST   "Use boost libraries from system" OFF)
option(SYSTEM_YAMLCPP "Use yaml-cpp library from system" OFF)
option(SYSTEM_PORTAUDIO "Use PortAudio libraries from system" ON)
if(WIN32 OR (APPLE AND SUPERNOVA))
    set(SYSTEM_PORTAUDIO OFF)
else()
    mark_as_advanced(SYSTEM_PORTAUDIO)
endif()

option(FORTIFY "Define -D_FORTIFY_SOURCE=2 to check for programming errors" OFF)

option(SC_HIDAPI "Build with HID support" ON)

option(SC_ABLETON_LINK "Build with Ableton Link support" ON)

if(APPLE)
    # codesigning used to break increamental builds in Xcode 11+, but it's now fixed
    option(SC_DISABLE_XCODE_CODESIGNING "Disable codesigning in XCode" OFF)
    # codesigning is also being broken by macdeployqt, but it is needed on arm64
    # to run the app locally at all, hence we sign after deploying the app
    option(SC_CODESIGN_AFTER_DEPLOY "Run ad hoc codesigning after creating the app bundle" ON)
    # verify_app fails with official Qt; we'll still use it for Qt from homebrew
    option(SC_VERIFY_APP "Run verify_app on the app bundle" OFF)
endif()

if(SC_DISABLE_XCODE_CODESIGNING)
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
endif()

#############################################
# Detect CCache

if(USE_CCACHE)

  # find ccache
  find_program(CCACHE_PROGRAM ccache)

  if(CCACHE_PROGRAM)

    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")

    if (MSVC)
      # workaround for Visual Studio adapted from https://github.com/ccache/ccache/wiki/MS-Visual-Studio#usage
      # NOTE: there is an issue with ccache installed from chocolatey
      # since chocolatey puts a "shim" as opposed to the actual executable in the PATH
      # the solution is to add the path to the actual ccache executable earlier in the path
      # e.g. in bash: export PATH=`echo c:/ProgramData/chocolatey/lib/ccache/tools/ccache*`:$PATH
      message(STATUS "Found ccache at ${CCACHE_PROGRAM}: using ccache with MSVC to speed up build process")

      file(COPY_FILE
        ${CCACHE_PROGRAM} ${CMAKE_BINARY_DIR}/cl.exe
        ONLY_IF_DIFFERENT)

      set(CMAKE_VS_GLOBALS
        "CLToolExe=cl.exe"
        "CLToolPath=${CMAKE_BINARY_DIR}"
        "TrackFileAccess=false"
        "UseMultiToolTask=true"
        "DebugInformationFormat=OldStyle"
      )
    else()
      if(CMAKE_GENERATOR STREQUAL "Xcode")
        message(STATUS "Found ccache at ${CCACHE_PROGRAM}: using ccache with Xcode to speed up build process")

        # workaround for using ccache with Xcode generator
        # thanks to Craig Scott: https://crascit.com/2016/04/09/using-ccache-with-cmake/

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
      else()
        message(STATUS "Found ccache at ${CCACHE_PROGRAM}: using ccache to speed up build process")
      endif() # Xcode
    endif() # MSVC
  endif() # ccache found
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
    # policy CMP0167 is only available from CMake 3.30 on
    if(POLICY CMP0167)
        # CMP0167 NEW policy: use Boost-provided BoostConfig.cmake config file
        # instead of the old FindBoost module
        cmake_policy(SET CMP0167 NEW)
    endif()
    set(Boost_USE_MULTITHREADED ON)
    set(BOOST_COMPONENTS thread program_options regex)
    if(ENABLE_TESTSUITE)
        list(APPEND BOOST_COMPONENTS unit_test_framework)
    endif()
    find_package(Boost 1.70.0 REQUIRED COMPONENTS ${BOOST_COMPONENTS})
    if (Boost_FOUND)
        message(STATUS "Found system Boost in ${Boost_INCLUDE_DIRS}")
    endif()
else()
	# bundled boost is set up in external_libraries/CMakeLists.txt
	# here we just set some global definitions
	if(WIN32)    
		#avoid unnecesary autolink
		add_definitions(-DBOOST_DATE_TIME_NO_LIB -DBOOST_ALL_NO_LIB)
		# use session manager to set shared memory location, see issue 2409
		add_definitions(-DBOOST_INTERPROCESS_BOOTSTAMP_IS_SESSION_MANAGER_BASED)
	else()
		add_definitions(-DBOOST_ASIO_DISABLE_VISIBILITY -DBOOST_DISABLE_EXPLICIT_SYMBOL_VISIBILITY)
	endif()
	add_definitions(-DBOOST_CHRONO_HEADER_ONLY -DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE)
endif() 

if(SYSTEM_YAMLCPP)
  find_package(YamlCpp)
else()
  set(SYSTEM_YAMLCPP OFF)
endif()

#############################################
# compiler settings - hardware

set(SC_COMPILER_ARCH_FLAGS "" CACHE STRING "Architecture specific compiler flags")

if("${SC_COMPILER_ARCH_FLAGS}" STREQUAL "")

	set(_detected_flags "")

	if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		if (NATIVE)
			list(APPEND _detected_flags "-march=native")
		else()
			if("x86_64" IN_LIST CMAKE_OSX_ARCHITECTURES AND "arm64" IN_LIST CMAKE_OSX_ARCHITECTURES) # universal binary
				list(APPEND _detected_flags "-Xarch_x86_64" "-march=haswell" "-Xarch_arm64" "-mcpu=apple-m1")
			elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
				list(APPEND _detected_flags "-march=haswell")
			elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
				list(APPEND _detected_flags "-mcpu=apple-m1")
			endif()
		endif()

	elseif(MSVC)
		if(CMAKE_CXX_COMPILER_ARCHITECTURE_ID MATCHES "x64" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.40")
			list(APPEND _detected_flags "/arch:SSE4.2")
		elseif(CMAKE_CXX_COMPILER_ARCHITECTURE_ID MATCHES "ARM64" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.40")
			list(APPEND _detected_flags "/arch:armv8.0")
		endif()

	elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
		if (NATIVE)
			list(APPEND _detected_flags "-march=native")
		else()
			if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64|AMD64")
				if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "12.0" OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "11.0"))
					list(APPEND _detected_flags "-march=x86-64-v2")
				else() # fallback for older compilers
					list(APPEND _detected_flags "-msse4.2")
				endif()
			elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
				list(APPEND _detected_flags "-march=armv8-a")
			elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7")
				list(APPEND _detected_flags "-march=armv7-a" "-mfpu=neon" "-mfloat-abi=hard")
			elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv6")
				list(APPEND _detected_flags "-march=armv6" "-mtune=arm1176jzf-s" "-mfpu=vfp" "-mfloat-abi=hard")
			else()
				message(STATUS "${CMAKE_CXX_COMPILER_ID}: No hardware optimization flags set for ${CMAKE_SYSTEM_PROCESSOR}")
			endif()
		endif()
	endif()

	if(NOT "${_detected_flags}" STREQUAL "")
		set(SC_COMPILER_ARCH_FLAGS "${_detected_flags}" CACHE STRING "Architecture specific compiler flags" FORCE)
	endif()

endif()

if(NOT "${SC_COMPILER_ARCH_FLAGS}" STREQUAL "")
	separate_arguments(SC_COMPILER_ARCH_FLAGS)
	message(STATUS "Compiler architecture flags: ${SC_COMPILER_ARCH_FLAGS}")
	add_compile_options(${SC_COMPILER_ARCH_FLAGS})
endif()

#############################################
# other compiler flags

set(CMAKE_CXX_STANDARD 17)

# set default symbol visibility
SET(CMAKE_CXX_VISIBILITY_PRESET hidden)
SET(CMAKE_C_VISIBILITY_PRESET hidden)
SET(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	add_compile_options(
		-fno-math-errno          # Optimize math functions by not setting errno
		-fno-signaling-nans      # Assume no signaling NaNs for better optimization
	)

	if(APPLE)
		add_compile_definitions("_REENTRANT")
	elseif(NOT WIN32)
		add_compile_options("-pthread")
	endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	if(NOT APPLE)
		add_compile_options(
			-fno-math-errno      # On Apple and most BSDs, math library functions never set errno, and so -fno-math-errno is the default
		)
	endif()
	if(NOT SC_CLANG_USES_LIBSTDCPP)
		# the default
		add_compile_options("-stdlib=libc++")
		add_link_options("-stdlib=libc++")
	endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	add_compile_options(
		-Werror=return-type      # Error out if function definitions lack a return type
		-fsigned-zeros           # Maintain distinction between -0.0 and +0.0
		-fno-associative-math    # Prevent reordering of floating-point operations
	)

	if(FORTIFY)
		add_compile_definitions("_FORTIFY_SOURCE=2")
	endif()

endif()

if(WIN32)
	add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX _WIN32_WINNT=0x0600)
endif()

if(MSVC)
  add_compile_options(
    /wd4100 # unreferenced parameter
    /wd4101 # unreferenced local variable
    /wd4102 # unreferenced label
    /wd4200 # nonstandard extension used: zero-sized array in struct/union
	/diagnostics:caret # better diagnostics, shows source code location in error repot
  )

# this flag causes MSVC to correctly report __cplusplus, which prevents a compiler error
# caused by some versions of libsndfile's C++ header redefining nullptr as NULL.
# See https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
  add_compile_options("/Zc:__cplusplus")

# for math operations, we expect the default /fp:precise flag to be used, so that we match settings for gcc/clang

endif(MSVC)

if(MINGW)
	add_compile_options(-mstackrealign)
endif()

if (SC_MEMORY_DEBUGGING)
	add_compile_definitions(DISABLE_MEMORY_POOLS ENABLE_MEMORY_CHECKS)
endif()

#############################################
# subdirectories
add_subdirectory(tools)
add_subdirectory(external_libraries)


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
add_subdirectory(langutils)
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
	set(SCCLASSLIB_EXCLUDE_REGEX "${SCCLASSLIB_EXCLUDE_REGEX}|/GUI$")
endif()

if(WIN32)
	set(SCCLASSLIB_EXCLUDE_REGEX "${SCCLASSLIB_EXCLUDE_REGEX}")
endif()

if(NOT SC_ABLETON_LINK)
	set(SCCLASSLIB_EXCLUDE_REGEX "${SCCLASSLIB_EXCLUDE_REGEX}|/Ableton$")
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
			install( CODE
				"EXECUTE_PROCESS(COMMAND ln -shF ${CMAKE_CURRENT_SOURCE_DIR}/SCClassLibrary/${onedirectory} ${CMAKE_INSTALL_PREFIX}/${auxresourcesdir}/SCClassLibrary/${onedirectory} )" )
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
			PATTERN "*~" EXCLUDE
			PATTERN "*#" EXCLUDE
		)
	endif()
endif()


#############################################
#
# build scdoc help files
#
list(APPEND BUILD_CLASSLIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/SCClassLibrary")

# this folder has an extension to Platform that disables the loadStartupFiles method
list(APPEND BUILD_CLASSLIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/platform/disable_startup_files")

foreach(arg ${BUILD_CLASSLIBRARIES})
	set(BUILD_CLASSLIBRARYPATH "${BUILD_CLASSLIBRARYPATH}  - ${arg}\n")
endforeach()

configure_file(build_sclang.cfg.in ${CMAKE_CURRENT_BINARY_DIR}/build_sclang.cfg)

if(SC_DOC_RENDER)
	file(GLOB_RECURSE SCDocSources RELATIVE HelpSource .*[^~])
	file(GLOB_RECURSE SCDocClasses RELATIVE SCClassLibrary/SCDoc *.sc)

	add_custom_target(doc ALL
		COMMAND sclang
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

set(CPACK_PACKAGE_VERSION_MAJOR ${SC_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${SC_VERSION_MINOR})
# No restriction on version strings unlike for project()
set(CPACK_PACKAGE_VERSION_PATCH ${SC_VERSION_PATCH}${SC_VERSION_TWEAK})

if(APPLE)
	set(CPACK_GENERATOR "DragNDrop")
	set(CPACK_DMG_FORMAT "UDBZ")
	set(CPACK_DMG_VOLUME_NAME "${scappbundlename}")
	set(CPACK_SYSTEM_NAME "OSX")
#	set(CPACK_PACKAGE_FILE_NAME "${scappbundlename}-${SC_VERSION}")
	set(CPACK_DMG_DS_STORE "${CMAKE_SOURCE_DIR}/package/ds_store")
	set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/package/background.png")
    set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/icons/sc_ide.icns")
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
