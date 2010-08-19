find_package(Sndfile)
find_package(Jack)

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
    SC_World.cpp)

include_directories(../../Headers/server)

set_property(DIRECTORY
             APPEND
             PROPERTY COMPILE_DEFINITIONS SC_MEMORY_ALIGNMENT=16)

if (JACK_FOUND)
	message(STATUS "Compile with Jack support")
	list(APPEND scsynth_sources SC_Jack.cpp)
	set_property(DIRECTORY
				 APPEND
				 PROPERTY COMPILE_DEFINITIONS SC_AUDIO_API=SC_AUDIO_API_JACK ${JACK_DEFINITIONS})
	include_directories(${JACK_INCLUDE_DIRS})
endif()

if (FINAL_BUILD)
	CREATE_FINAL_FILE(libscsynth_final.cpp ${scsynth_sources})
	add_library(libscsynth SHARED libscsynth_final.cpp)
else()
	add_library(libscsynth SHARED ${scsynth_sources})
endif()

target_link_libraries(libscsynth sccommon)

if(SNDFILE_FOUND)
	include_directories(${SNDFILE_INCLUDE_DIR})
	target_link_libraries(libscsynth ${SNDFILE_LIBRARIES})
else()
	message(SEND_ERROR "Cannot find libsndfile")
endif(SNDFILE_FOUND)

if (JACK_FOUND)
	target_link_libraries(libscsynth ${JACK_LIBRARIES})
endif()

set_property(TARGET libscsynth
			 PROPERTY OUTPUT_NAME scsynth)

add_executable(scsynth scsynth_main.cpp)
target_link_libraries(scsynth libscsynth)

