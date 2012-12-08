include_directories(${CMAKE_SOURCE_DIR}/include/common
                    ${CMAKE_SOURCE_DIR}/include/lang
                    ${CMAKE_SOURCE_DIR}/include/plugin_interface
                    ${CMAKE_SOURCE_DIR}/include/server
                    ${CMAKE_SOURCE_DIR}/common

                    ${YAMLCPP_INCLUDE_DIR}

                    ${CMAKE_SOURCE_DIR}/external_libraries/boost-lockfree
                    ${CMAKE_SOURCE_DIR}/external_libraries/threadpool
                    ${CMAKE_SOURCE_DIR}/external_libraries/TLSF-2.4.6/src
                    LangSource/Bison)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    include_directories(${CMAKE_SOURCE_DIR}/external_libraries/boost)
endif()

if(APPLE)
	include_directories("LangPrimSource/${HID_UTIL_DIR}")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	find_package(ALSA)
elseif(WIN32)
	find_package(Portmidi)
endif()

find_package(Readline 5.0)

find_package(Sndfile)

set(sclang_sources
	LangPrimSource/SC_Wii.cpp
	LangPrimSource/PyrSignalPrim.cpp
	LangPrimSource/PyrSched.cpp
	LangPrimSource/PyrPrimitive.cpp
	LangPrimSource/PyrMathPrim.cpp
	LangPrimSource/SC_ComPort.cpp
	LangPrimSource/OSCData.cpp
	LangPrimSource/PyrArchiver.cpp
	LangPrimSource/PyrArrayPrimitives.cpp
	LangPrimSource/PyrBitPrim.cpp
	LangPrimSource/PyrCharPrim.cpp
	LangPrimSource/PyrFilePrim.cpp
	LangPrimSource/PyrListPrim.cpp
	LangPrimSource/PyrPlatformPrim.cpp
	LangPrimSource/PyrStringPrim.cpp
	LangPrimSource/PyrSymbolPrim.cpp
	LangPrimSource/PyrUnixPrim.cpp
	LangSource/AdvancingAllocPool.cpp
	LangSource/ByteCodeArray.cpp
	LangSource/DumpParseNode.cpp
	LangSource/GC.cpp
	LangSource/InitAlloc.cpp
	LangSource/PyrInterpreter3.cpp
	LangSource/PyrLexer.cpp
	LangSource/PyrMathOps.cpp
	LangSource/PyrMathSupport.cpp
	LangSource/PyrMessage.cpp
	LangSource/PyrObject.cpp
	LangSource/PyrParseNode.cpp
	LangSource/PyrSignal.cpp
	LangSource/PyrSymbolTable.cpp
	LangSource/SC_LanguageClient.cpp
	LangSource/SC_LanguageConfig.cpp
	LangSource/SC_TerminalClient.cpp
	LangSource/Samp.cpp
	LangSource/SimpleStack.cpp
	LangSource/VMGlobals.cpp
	LangSource/dumpByteCodes.cpp

	${CMAKE_SOURCE_DIR}/common/fftlib.c
	${CMAKE_SOURCE_DIR}/common/SC_AllocPool.cpp
	${CMAKE_SOURCE_DIR}/common/SC_DirUtils.cpp
	${CMAKE_SOURCE_DIR}/common/SC_Reply.cpp
	${CMAKE_SOURCE_DIR}/common/SC_Sem.cpp
	${CMAKE_SOURCE_DIR}/common/SC_StandAloneInfo_Darwin.cpp
	${CMAKE_SOURCE_DIR}/common/SC_StringBuffer.cpp
	${CMAKE_SOURCE_DIR}/common/SC_StringParser.cpp
	${CMAKE_SOURCE_DIR}/common/SC_TextUtils.cpp

	${CMAKE_SOURCE_DIR}/common/scsynthsend.cpp
	${CMAKE_SOURCE_DIR}/common/sc_popen.cpp
)

if (NOT Boost_FOUND)
	file(GLOB boost_system_sources ../external_libraries/boost/libs/system/src/*cpp)
	file(GLOB boost_fs_sources ../external_libraries/boost/libs/filesystem/src/*cpp)
	file(GLOB boost_regex_sources ../external_libraries/boost/libs/regex/src/*cpp)
	list(APPEND sclang_sources ${boost_fs_sources} ${boost_system_sources} ${boost_regex_sources})
endif()

file(GLOB_RECURSE headers ../include/*.h* )
file(GLOB_RECURSE external_headers ../external_libraries/*.h* )
list(APPEND sclang_sources ${headers} ${external_headers}) # make qt creator happy

if(NOT WIN32)
	list(APPEND sclang_sources LangPrimSource/PyrSerialPrim.cpp)
endif()

if(WIN32)
	list(APPEND sclang_sources ${CMAKE_SOURCE_DIR}/common/SC_Win32Utils.cpp)
endif()

if(WIN32)
	include_directories(../platform/windows/compat_stuff)
	list(APPEND sclang_sources ../platform/windows/compat_stuff/getopt.c)
endif()

set(sclang_parser_source LangSource/Bison/lang11d_tab.cpp)

add_definitions(-DYYSTACK_USE_ALLOCA)

if(UNIX)
	if(APPLE)
		list(APPEND sclang_sources
			LangPrimSource/WiiMote_OSX/wiiremote.c
			LangPrimSource/SC_HID.cpp
			LangPrimSource/${HID_UTIL_DIR}/HID_Utilities.c
			LangPrimSource/${HID_UTIL_DIR}/HID_Name_Lookup.c
			LangPrimSource/${HID_UTIL_DIR}/HID_Queue_Utilities.c
			LangPrimSource/${HID_UTIL_DIR}/HID_Error_Handler.c
			LangPrimSource/SC_CoreMIDI.cpp
			LangPrimSource/SC_CoreAudioPrim.cpp
			LangPrimSource/SC_Speech.M
			)
		add_definitions(-DHAVE_SPEECH)
		if (CMAKE_COMPILER_IS_GNUCXX)
			set_source_files_properties(LangPrimSource/SC_Speech.M
										PROPERTIES COMPILE_FLAGS "-x objective-c++ -fobjc-exceptions")
		endif()
		if(NOT (HID_UTIL_DIR MATCHES "HID_Utilities_10_4"))
			list(APPEND sclang_sources
				LangPrimSource/${HID_UTIL_DIR}/IOHIDElement_.c
				LangPrimSource/${HID_UTIL_DIR}/IOHIDDevice_.c
			)
		endif()
	else(APPLE)
		if(ALSA_FOUND)
			list(APPEND sclang_sources LangPrimSource/SC_AlsaMIDI.cpp)
		endif()
		list(APPEND sclang_sources LangPrimSource/SC_LID.cpp)
		add_definitions(-DHAVE_LID)
	endif(APPLE)
else(UNIX)
endif(UNIX)

if(WIN32)
	include_directories (../platform/windows/compat_stuff)
	list(APPEND sclang_sources ../platform/windows/compat_stuff/getopt.c)
	if(PORTMIDI_FOUND)
		include_directories (${PORTMIDI_INCLUDE_DIRS})
		add_definitions(-DHAVE_PORTMIDI)
		list(APPEND sclang_sources LangPrimSource/SC_PortMidi.cpp)
	endif()
endif()

if (SC_WII OR APPLE)
	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		find_package(CWiid)
		find_package(Bluetooth)

		if (BLUETOOTH_FOUND AND CWIID_FOUND)
			add_definitions(-DHAVE_WII)
			include_directories(${CWIID_INCLUDE_DIRS} ${BLUETOOTH_INCLUDE_DIRS})
			message(STATUS "Compiling with WiiMote support")
		else()
			message(SEND_ERROR "Cannot find libcwiid or libbluetooth.\n (If Wii support is not required, then set SC_WII=no)")
		endif()
	elseif(APPLE)
		add_definitions(-DHAVE_WII)
		list(APPEND sclang_sources LangPrimSource/WiiMote_OSX/wiiremote.c)
		include_directories(LangPrimSource/WiiMote_OSX)
	endif()
endif()

include(../SCDoc/CMakeLists.txt)
list(APPEND sclang_sources ${SCDOC_SRCS})

if(SC_IDE)
	list(APPEND sclang_sources ../editors/sc-ide/primitives/sc_ipc_client.cpp)
endif()

if(SC_QT)
	set(QT_COLLIDER_LANG_CLIENT ON)
	include(../QtCollider/CMakeLists.txt)
	list(APPEND sclang_sources ${QT_COLLIDER_SRCS})
endif()

if(0 AND FINAL_BUILD) # sclang final-builds are broken
	CREATE_FINAL_FILE(libsclang_final.cpp ${sclang_sources})
	add_library(libsclang STATIC libsclang_final.cpp ${sclang_parser_source})
else()
	add_library(libsclang STATIC ${sclang_sources} ${sclang_parser_source})
endif()

set_property(TARGET libsclang
	APPEND
	PROPERTY COMPILE_DEFINITIONS BUILDING_SUPERCOLLIDER)

if (SC_WII OR APPLE)
	if (BLUETOOTH_FOUND AND CWIID_FOUND)
		target_link_libraries(libsclang ${BLUETOOTH_LIBRARIES} ${CWIID_LIBRARIES})
	elseif(APPLE)
		target_link_libraries(libsclang "-framework IOBluetooth")
	endif()
endif()

if (Boost_FOUND)
	target_link_libraries(libsclang ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
endif()

if (SCLANG_SERVER)
	target_link_libraries(libsclang libscsynth)
else()
	set_property(TARGET libsclang
		APPEND
		PROPERTY COMPILE_DEFINITIONS NO_INTERNAL_SERVER
		)
endif()


set_property(TARGET libsclang
	PROPERTY OUTPUT_NAME sclang)


## external libraries
if(READLINE_FOUND)
	message(STATUS "Compiling with Readline support")
	set_property(TARGET libsclang
		APPEND
		PROPERTY COMPILE_DEFINITIONS HAVE_READLINE)
	include_directories(${READLINE_INCLUDE_DIR})
	target_link_libraries(libsclang ${READLINE_LIBRARY})
endif(READLINE_FOUND)

if (APPLE)
	target_link_libraries(libsclang "-framework Carbon")
	target_link_libraries(libsclang "-framework CoreAudio")
	target_link_libraries(libsclang "-framework CoreMIDI")
	target_link_libraries(libsclang "-framework IOKit")
	target_link_libraries(libsclang "-framework CoreServices")
endif()

if(ALSA_FOUND)
	message(STATUS "Compiling with ALSA midi support")
	set_property(TARGET libsclang
		APPEND
		PROPERTY COMPILE_DEFINITIONS HAVE_ALSA=1)
	target_link_libraries(libsclang ${ALSA_LIBRARY})
endif(ALSA_FOUND)

if(SNDFILE_FOUND)
	include_directories(${SNDFILE_INCLUDE_DIR})
	target_link_libraries(libsclang ${SNDFILE_LIBRARIES})
elseif(NOT NO_LIBSNDFILE)
	message(SEND_ERROR "Cannot find libsndfile")
endif(SNDFILE_FOUND)

if (FFTW3F_FOUND)
	include_directories (${FFTW3F_INCLUDE_DIR})
	target_link_libraries(libsclang ${FFTW3F_LIBRARY})
endif()

if (WIN32)
	target_link_libraries(libsclang wsock32 ws2_32)
	if(PORTMIDI_FOUND)
		target_link_libraries(libsclang ${PORTMIDI_LIBRARIES})
	endif()
endif()

if (GC_SANITYCHECK)
	add_definitions(-DGC_SANITYCHECK)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(libsclang rt)
endif()

target_link_libraries(libsclang ${PTHREADS_LIBRARY} ${YAMLCPP_LIBRARY})

if(SC_QT)
	target_link_libraries(libsclang ${QT_COLLIDER_LIBS})
endif()

if(SC_IDE)
	add_definitions(-DSC_IDE -DQT_NO_KEYWORDS)
	find_package( Qt4 4.7 REQUIRED QtCore QtNetwork )
	include( ${QT_USE_FILE} )
	target_link_libraries( libsclang ${QT_LIBRARIES} )
endif()

add_executable(sclang LangSource/cmdLineFuncs.cpp)

if (Boost_FOUND)
	target_link_libraries(sclang libsclang ${Boost_THREAD_LIBRARY})
else()
	target_link_libraries(sclang libsclang boost_thread)
endif()

set_property(TARGET sclang
	APPEND
	PROPERTY COMPILE_DEFINITIONS USE_SC_TERMINAL_CLIENT BUILDING_SUPERCOLLIDER)

if(LTO)
	set_property(TARGET sclang libsclang
					APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

	set_property(TARGET sclang libsclang
					APPEND PROPERTY LINK_FLAGS "-flto -flto-report -fwhole-program")
endif()

if(APPLE)
	# determines the app name and app install location (scappbundlename, scappdir):
	include (${CMAKE_SOURCE_DIR}/cmake_modules/MacAppFolder.cmake)

	install(TARGETS sclang
			DESTINATION "${scappauxresourcesdir}"
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
elseif(WIN32)
	install(TARGETS sclang
			DESTINATION "SuperCollider"
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
else()
	install(TARGETS sclang
			RUNTIME DESTINATION "bin"
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()
