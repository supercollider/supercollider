find_package(Sndfile)

# Here we work out which audio API to use, from system type and/or user option.
string(TOLOWER "$ENV{AUDIOAPI}" AUDIOAPI)
if(AUDIOAPI STREQUAL "")
	if(APPLE)
		set(AUDIOAPI coreaudio)
	elseif(WIN32)
		set(AUDIOAPI portaudio)
	else()
		set(AUDIOAPI jack)
	endif(APPLE)
endif()

if(NOT (AUDIOAPI MATCHES "^(jack|coreaudio|portaudio)$"))
	message(WARNING "Unrecognised audio API: ${AUDIOAPI}")
endif()

if(AUDIOAPI STREQUAL jack)
	find_package(Jack)
	if(NOT JACK_FOUND)
		message(FATAL_ERROR "Jack selected as audio API, but development files not found")
	endif()
elseif(AUDIOAPI STREQUAL portaudio)
	find_package(Portaudio)
	if(NOT PORTAUDIO_FOUND)
		message(FATAL_ERROR "Portaudio selected as audio API, but development files not found")
	endif()
endif()
message(STATUS "Audio API: ${AUDIOAPI}")

set(scsynth_sources
    Samp.cpp
    SC_BufGen.cpp
    SC_Carbon.cpp
    SC_Complex.cpp
    SC_ComPort.cpp
    SC_CoreAudio.cpp
    SC_Errors.cpp
    SC_Graph.cpp
    SC_GraphDef.cpp
    SC_Group.cpp
    SC_Lib_Cintf.cpp
    SC_Lib.cpp
    SC_MiscCmds.cpp
    SC_Node.cpp
    SC_Rate.cpp
    SC_SequencedCommand.cpp
    SC_Str4.cpp
    SC_SyncCondition.cpp
    SC_Unit.cpp
    SC_UnitDef.cpp
    SC_World.cpp
    Rendezvous.cpp)

include_directories(../../Headers/server)

set_property(DIRECTORY
             APPEND
             PROPERTY COMPILE_DEFINITIONS SC_MEMORY_ALIGNMENT=16)

if (AUDIOAPI STREQUAL jack)
	list(APPEND scsynth_sources SC_Jack.cpp)
	add_definitions("-DSC_AUDIO_API=SC_AUDIO_API_JACK" ${JACK_DEFINITIONS})
	include_directories(${JACK_INCLUDE_DIRS})
elseif (AUDIOAPI STREQUAL portaudio)
	add_definitions("-DSC_AUDIO_API=SC_AUDIO_API_PORTAUDIO" ${PORTAUDIO_DEFINITIONS})
	include_directories(${PORTAUDIO_INCLUDE_DIRS})
endif()

if (FINAL_BUILD)
	CREATE_FINAL_FILE(libscsynth_final.cpp ${scsynth_sources})
	add_library(libscsynth SHARED libscsynth_final.cpp)
else()
	add_library(libscsynth SHARED ${scsynth_sources})
endif()

target_link_libraries(libscsynth sccommon)

if(CURL)
	find_package(Curl)
	if(CURL_FOUND)
		target_link_libraries(libscsynth ${CURL_LIBRARIES})
	endif()
endif()

if(SNDFILE_FOUND)
	include_directories(${SNDFILE_INCLUDE_DIR})
	target_link_libraries(libscsynth ${SNDFILE_LIBRARIES})
elseif(NOT NO_LIBSNDFILE)
	message(SEND_ERROR "Cannot find libsndfile")
endif(SNDFILE_FOUND)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_definitions(-DSC_PLUGIN_DIR="${CMAKE_INSTALL_PREFIX}/lib/SuperCollider/plugins")
elseif(NOT APPLE)
	message(SEND_ERROR "Not implemented")
endif()

if (AUDIOAPI STREQUAL jack)
	target_link_libraries(libscsynth ${JACK_LIBRARIES})
elseif(AUDIOAPI STREQUAL portaudio)
	target_link_libraries(libscsynth ${PORTAUDIO_LIBRARIES})
elseif(AUDIOAPI STREQUAL coreaudio)
	target_link_libraries(libscsynth "-framework CoreAudio")
endif()

set_property(TARGET libscsynth
			 PROPERTY OUTPUT_NAME scsynth)

add_executable(scsynth scsynth_main.cpp)
target_link_libraries(scsynth libscsynth)


#TODO allow apple users to install to fhs-ish path if INSTALL_FHS option is enabled

if(APPLE)
	# determines the app name and app install location (scappbundlename, scappdir):
	include (../../cmake_modules/MacAppFolder.cmake)

	install(TARGETS libscsynth
			DESTINATION ${scappauxresourcesdir}
			PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE OWNER_WRITE
			COMPONENT app)

	install(TARGETS scsynth
			RUNTIME DESTINATION ${scappauxresourcesdir}
			PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE OWNER_WRITE
			COMPONENT app)
else()
	install(TARGETS libscsynth
			DESTINATION "lib"
			PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

	install(TARGETS scsynth
			RUNTIME DESTINATION "bin"
			PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()
