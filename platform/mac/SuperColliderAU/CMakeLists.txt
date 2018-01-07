cmake_minimum_required(VERSION 2.8.11) # needed for CFBundle building

if(NOT APPLE)
	message(SEND_ERROR "SuperColliderAU folder invoked by cmake, but not APPLE platform")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(CMAKE_COMPILER_IS_CLANG 1)
else()
	message(SEND_ERROR "Only CLANG compiler supported")
endif()

message("Building SuperColliderAU")

option(SSE "Compile with support for SSE instructions." ON)
option(SSE2 "Compile with support for SSE2 instructions." ON)

#build from SCAU dir
set(SC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

set(PLUGINS_DIR ${SC_DIR}/build/server/plugins/${CMAKE_BUILD_TYPE}/)

if (NOT EXISTS ${PLUGINS_DIR})
	message(SEND_ERROR "Pluigns directory not found: ${PLUGINS_DIR}")
endif()

file(GLOB PLUGINS ${SC_DIR}/build/server/plugins/${CMAKE_BUILD_TYPE}/*.scx)

include (${SC_DIR}/cmake_modules/FinalFile.cmake)
include (${SC_DIR}/cmake_modules/FindPthreads.cmake)
include (${SC_DIR}/cmake_modules/FindSndfile.cmake)

find_package(Pthreads)
find_package(Sndfile)

if (NOT PTHREADS_FOUND)
		message(SEND_ERROR "cannot find libpthreads")
endif()

if (NOT PTHREADS_FOUND)
		message(SEND_ERROR "cannot find libpthreads")
endif()

if (NOT SNDFILE_FOUND)
		message(SEND_ERROR "cannot find libsndfile")
endif()

add_subdirectory(${SC_DIR}/external_libraries ${CMAKE_BINARY_DIR}/external_libraries)

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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -stdlib=libc++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(scau_bundle "SuperColliderAU")
set(SNDFILE_INCLUDE_DIR ${SC_DIR}/external_libraries/libsndfile/)
set(BOOST_INCLUDE_DIR ${SC_DIR}external_libraries/boost/)
set(TLSF_INCLUDE_DIR ${SC_DIR}/external_libraries/TLSF-2.4.6/src/)

include_directories(${SC_DIR}/external_libraries
	${SC_DIR}/${BOOST_INCLUDE_DIR}
	${SNDFILE_INCLUDE_DIR}
	${PTHREADS_INCLUDE_DIR}
	${TLSF_INCLUDE_DIR}
	${SC_DIR}/external_libraries/boost_sync/include
	${SC_DIR}/external_libraries/boost_lockfree
	${SC_DIR}/external_libraries/nova-simd
	${SC_DIR}/external_libraries/nova-tt
	)

set_property(DIRECTORY
	APPEND
	PROPERTY COMPILE_DEFINITIONS SC_MEMORY_ALIGNMENT=32)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/Source
  ${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/OtherBases
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/AUBase
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/Utility
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/PublicUtility
	${SC_DIR}/include/common
	${SC_DIR}/common
	${SC_DIR}/build/common
	${SC_DIR}/include/server/
	${SC_DIR}/include/plugin_interface
	${SC_DIR}/server/scsynth/
	)

add_definitions("-DSC_AUDIO_API=SC_AUDIO_API_AUDIOUNITS" ${AUDIOUNITS_DEFINITIONS})
add_definitions("-DSC_DARWIN")
add_definitions("-D__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES=0")
add_definitions(-w)# TODO: fix deprecation warnings

if (NOT Boost_FOUND)
	file(GLOB boost_system_sources ../../external_libraries/boost/libs/system/src/*cpp)
	file(GLOB boost_filesystem_sources ../../external_libraries/boost/libs/filesystem/src/*cpp)
endif()

set(scau_sources
	${SC_DIR}/common/fftlib.c
	${SC_DIR}/common/SC_fftlib.cpp
	${SC_DIR}/common/SC_AllocPool.cpp
	${SC_DIR}/common/SC_Errors.cpp
	${SC_DIR}/common/Samp.cpp
	${SC_DIR}/common/SC_StringBuffer.cpp
	${SC_DIR}/common/SC_StringParser.cpp
	${SC_DIR}/common/sc_popen.cpp
	${SC_DIR}/common/strtod.c
	${SC_DIR}/common/SC_Reply.cpp
	${SC_DIR}/common/SC_Filesystem_macos.cpp
	${SC_DIR}/common/SC_Filesystem_iphone.cpp
	${SC_DIR}/external_libraries/TLSF-2.4.6/src/tlsf.c
	${SC_DIR}/server/scsynth/SC_AU.cpp
	${SC_DIR}/server/scsynth/SC_BufGen.cpp
	${SC_DIR}/server/scsynth/SC_ComPort.cpp
	${SC_DIR}/server/scsynth/SC_CoreAudio.cpp
	${SC_DIR}/server/scsynth/SC_Graph.cpp
	${SC_DIR}/server/scsynth/SC_GraphDef.cpp
	${SC_DIR}/server/scsynth/SC_Group.cpp
	${SC_DIR}/server/scsynth/SC_Lib_Cintf.cpp
	${SC_DIR}/server/scsynth/SC_Lib.cpp
	${SC_DIR}/server/scsynth/SC_MiscCmds.cpp
	${SC_DIR}/server/scsynth/SC_Node.cpp
	${SC_DIR}/server/scsynth/SC_Rate.cpp
	${SC_DIR}/server/scsynth/SC_SequencedCommand.cpp
	${SC_DIR}/server/scsynth/SC_Str4.cpp
	${SC_DIR}/server/scsynth/SC_Unit.cpp
	${SC_DIR}/server/scsynth/SC_UnitDef.cpp
	${SC_DIR}/server/scsynth/Rendezvous.cpp
	${SC_DIR}/server/scsynth/SC_World.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Source/SuperColliderAU.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/AUBase/AUBase.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/OtherBases/AUMIDIBase.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/OtherBases/AUMIDIEffectBase.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/Utility/AUBuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/AUBase/AUDispatch.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/OtherBases/AUEffectBase.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/AUBase/AUInputElement.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/AUBase/AUPlugInDispatch.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/AUBase/AUOutputElement.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/AUBase/AUScopeElement.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/PublicUtility/CAAudioChannelLayout.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/PublicUtility/CAStreamBasicDescription.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/PublicUtility/CAVectorUnit.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/AUBase/ComponentBase.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/AUBase/AUPlugInDispatch.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Source/ControlSpecs.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Source/OSCMessages.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Source/Resources.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Source/SCProcess.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Source/SuperColliderAU.r
	${CMAKE_CURRENT_SOURCE_DIR}/Resources/Info.plist
	${CMAKE_CURRENT_SOURCE_DIR}/Resources/English.lproj/InfoPlist.strings
	${CMAKE_CURRENT_SOURCE_DIR}/Resources/serverConfig.plist
	${CMAKE_CURRENT_SOURCE_DIR}/Resources/SuperColliderAU.rsrc
	${boost_system_sources}
	${boost_filesystem_sources}
	${PLUGINS}
	)

if(APPLE)
	set_property(SOURCE ${SC_DIR}/common/SC_Filesystem_macos.cpp PROPERTY COMPILE_FLAGS -xobjective-c++)

	list(APPEND scau_sources
		${SC_DIR}/common/SC_Apple.mm
		)
	set_source_files_properties( ${SC_DIR}/common/SC_Apple.mm
		PROPERTIES COMPILE_FLAGS "-x objective-c++ -fobjc-exceptions"
		)
endif()

add_library(${scau_bundle} MODULE ${scau_sources})

find_program(REZ Rez NO_DEFAULT_PATHS)
execute_process(COMMAND
	${REZ}
	${CMAKE_CURRENT_SOURCE_DIR}/Source/SuperColliderAU.r
	-d SystemSevenOrLater=1 -useDF -script Roman -arch x86_64
	-o ${CMAKE_CURRENT_SOURCE_DIR}/Resources/SuperColliderAU.rsrc
	-I /System/Library/Frameworks/CoreServices.framework/Frameworks/CarbonCore.framework/Versions/A/Headers
	-I ${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/OtherBases/
	-I ${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses/CoreAudio/AudioUnits/AUPublic/AUBase/
	-I ${CMAKE_CURRENT_SOURCE_DIR}/CoreAudioUtilityClasses
	)

set_source_files_properties(
	${CMAKE_CURRENT_SOURCE_DIR}/Resources/SuperColliderAU.rsrc
	PROPERTIES GENERATED 1
)

set_target_properties(${scau_bundle} PROPERTIES
	BUNDLE 1
	BUNDLE_EXTENSION component
	XCODE_ATTRIBUTE_WRAPPER_EXTENSION component
	XCODE_ATTRIBUTE_MACH_O_TYPE mh_bundle
	XCODE_ATTRIBUTE_INFOPLIST_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Info.plist
	MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Info.plist
	LINK_FLAGS "-Wl")

set_source_files_properties(
	${CMAKE_CURRENT_SOURCE_DIR}/Resources/English.lproj/InfoPlist.strings
	PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/English.lproj")

set_source_files_properties(
	${CMAKE_CURRENT_SOURCE_DIR}/Resources/serverConfig.plist
	${CMAKE_CURRENT_SOURCE_DIR}/Resources/SuperColliderAU.rsrc
	PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

set_source_files_properties(
	${PLUGINS}
	PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/plugins")

set_source_files_properties(
	${CMAKE_CURRENT_SOURCE_DIR}/Resources/synthdefs/.
	PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/synthdefs")

target_link_libraries(${scau_bundle} "-framework AudioUnit")
target_link_libraries(${scau_bundle} "-framework CoreAudio")
target_link_libraries(${scau_bundle} "-framework CoreMIDI")
target_link_libraries(${scau_bundle} "-framework CoreServices")
#target_link_libraries(${scau_bundle} "-framework vecLib")
target_link_libraries(${scau_bundle} "-framework Accelerate")
target_link_libraries(${scau_bundle} "-framework Foundation")
target_link_libraries(${scau_bundle} ${SNDFILE_LIBRARIES})
add_definitions("-DLIBSNDFILE_1018")

if (Boost_FOUND)
	target_link_libraries(${scau_bundle} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} )
else()
	target_link_libraries(${scau_bundle} boost_system boost_filesystem)
endif()

target_link_libraries(${scau_bundle} ${PTHREADS_LIBRARY})
