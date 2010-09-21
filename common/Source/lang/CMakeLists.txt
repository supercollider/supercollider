include_directories(../../Headers/lang
	LangSource/Bison)

find_package(ALSA)
find_package(Readline)
find_package(ICU)
find_package(Sndfile)

if (NOT ICU_FOUND)
	message(SEND_ERROR "cannot find ICU")
endif()

if (SC_QT)
  add_definitions( -DSC_QT )
endif (SC_QT)

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
	LangPrimSource/PyrSerialPrim.cpp
	LangPrimSource/PyrStringPrim.cpp
	LangPrimSource/PyrUStringPrim.cpp
	LangPrimSource/PyrSymbolPrim.cpp
	LangPrimSource/PyrUnixPrim.cpp
	LangSource/AdvancingAllocPool.cpp
	LangSource/ByteCodeArray.cpp
	LangSource/DumpParseNode.cpp
	LangSource/GC.cpp
	LangSource/InitAlloc.cpp
	LangSource/PyrFileUtils.cpp
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
	LangSource/SC_LibraryConfig.cpp
	LangSource/SC_TerminalClient.cpp
	LangSource/Samp.cpp
	LangSource/SimpleStack.cpp
	LangSource/VMGlobals.cpp
	LangSource/dumpByteCodes.cpp
	../common/fftlib.c)

set(sclang_parser_source LangSource/Bison/lang11d_tab.cpp)

if(UNIX)
	if(APPLE)
		list(APPEND sclang_sources LangPrimSource/WiiMote_OSX/wiiremote.c)
	else(APPLE)
		if(ALSA_FOUND)
			list(APPEND sclang_sources LangPrimSource/SC_AlsaMIDI.cpp)
		endif()
		list(APPEND sclang_sources LangPrimSource/SC_LID.cpp)
	endif(APPLE)
else(UNIX)
endif(UNIX)

if(FINAL_BUILD)
	CREATE_FINAL_FILE(libsclang_final.cpp ${sclang_sources})
	add_library(libsclang SHARED libsclang_final.cpp ${sclang_parser_source})
else()
	add_library(libsclang SHARED ${sclang_sources} ${sclang_parser_source})
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
	target_link_libraries(libsclang ${READLINE_LIBRARY})
endif(READLINE_FOUND)

include_directories(${ICU_INCLUDE_DIRS})
target_link_libraries(libsclang ${ICU_LIBRARIES} ${ICU_I18N_LIBRARIES})

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
else()
	message(SEND_ERROR "Cannot find libsndfile")
endif(SNDFILE_FOUND)

if (SC_QT)
  target_link_libraries(libsclang QtCollider)
endif (SC_QT)

target_link_libraries(libsclang sccommon pthread)

add_executable(sclang LangSource/cmdLineFuncs.cpp)
target_link_libraries(sclang libsclang)

set_property(TARGET sclang
	APPEND
	PROPERTY COMPILE_DEFINITIONS USE_SC_TERMINAL_CLIENT)


install(TARGETS libsclang
		DESTINATION "lib"
		PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install(TARGETS sclang
		DESTINATION "bin"
		PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
