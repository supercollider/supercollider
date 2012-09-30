if(NOT APPLE)
	message(SEND_ERROR "mac folder invoked by cmake, but not APPLE platform")
endif()

# determines the app name and app install location (scappbundlename, scappdir):
include (${CMAKE_SOURCE_DIR}/cmake_modules/MacAppFolder.cmake)

# On OSX, everything is installed in a folder named after ${scappbundlename},
# which defaults to "SuperCollider". The folder is installed under ${CMAKE_INSTALL_PREFIX},
# which defaults to <build directory>/Install/
# This means the app will go in <build dir>/Install/SuperCollider/SuperCollider.app
# Class lib and similar goes into .app/Contents/Resources, as well as the external sclang and scsynth binaries.
# The contents of the install prefix folder will be packaged to a DMG with "make package"

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/Install" CACHE PATH "Install path prefix" FORCE)
endif()
message(STATUS "Install prefix ${CMAKE_INSTALL_PREFIX}")

include_directories(
	${CMAKE_SOURCE_DIR}/lang/LangSource/Bison
	${CMAKE_SOURCE_DIR}/include/common
	${CMAKE_SOURCE_DIR}/include/lang
	${CMAKE_SOURCE_DIR}/include/plugin_interface
	${CMAKE_SOURCE_DIR}/include/server
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/editors/scapp/include
	${CMAKE_SOURCE_DIR}/editors/scapp/include/SMLAdvancedFind
	${CMAKE_SOURCE_DIR}/editors/scapp/include/SMLAdvancedFind/ICU
	${CMAKE_SOURCE_DIR}/external_libraries/icu/
	${CMAKE_SOURCE_DIR}/external_libraries/icu/unicode/
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/${HID_UTIL_DIR}
	${CMAKE_SOURCE_DIR}/external_libraries/boost-lockfree
	${CMAKE_SOURCE_DIR}/external_libraries/boost
	${CMAKE_SOURCE_DIR}/external_libraries/threadpool
	${YAMLCPP_INCLUDE_DIR}

	${CMAKE_SOURCE_DIR}/external_libraries/TLSF-2.4.6/src
	)

find_package(Sndfile)

set(scapp_sources
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/SC_Wii.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/PyrSignalPrim.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/PyrSched.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/PyrPrimitive.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/PyrMathPrim.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/SC_ComPort.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/OSCData.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/PyrArchiver.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/PyrArrayPrimitives.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/PyrBitPrim.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/PyrCharPrim.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/PyrFilePrim.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/PyrListPrim.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/PyrPlatformPrim.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/PyrSerialPrim.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/PyrStringPrim.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/PyrSymbolPrim.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/PyrUnixPrim.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/SC_Speech.M
	${CMAKE_SOURCE_DIR}/lang/LangSource/AdvancingAllocPool.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/ByteCodeArray.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/DumpParseNode.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/GC.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/InitAlloc.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/PyrInterpreter3.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/PyrLexer.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/PyrMathOps.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/PyrMathSupport.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/PyrMessage.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/PyrObject.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/PyrParseNode.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/PyrSignal.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/PyrSymbolTable.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/SC_LanguageConfig.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/Samp.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/SimpleStack.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/VMGlobals.cpp
	${CMAKE_SOURCE_DIR}/lang/LangSource/dumpByteCodes.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/WiiMote_OSX/wiiremote.c
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/SC_HID.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/${HID_UTIL_DIR}/HID_Utilities.c
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/${HID_UTIL_DIR}/HID_Name_Lookup.c
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/${HID_UTIL_DIR}/HID_Queue_Utilities.c
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/${HID_UTIL_DIR}/HID_Error_Handler.c
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/SC_CoreMIDI.cpp
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/SC_CoreAudioPrim.cpp

	${CMAKE_SOURCE_DIR}/common/fftlib.c
	${CMAKE_SOURCE_DIR}/common/SC_fftlib.cpp
	${CMAKE_SOURCE_DIR}/common/SC_AllocPool.cpp
	${CMAKE_SOURCE_DIR}/common/SC_DirUtils.cpp
	${CMAKE_SOURCE_DIR}/common/SC_Sem.cpp
	${CMAKE_SOURCE_DIR}/common/SC_StandAloneInfo_Darwin.cpp
	${CMAKE_SOURCE_DIR}/common/SC_StringBuffer.cpp
	${CMAKE_SOURCE_DIR}/common/SC_StringParser.cpp
	${CMAKE_SOURCE_DIR}/common/SC_TextUtils.cpp

	${CMAKE_SOURCE_DIR}/common/scsynthsend.cpp
	${CMAKE_SOURCE_DIR}/common/sc_popen.cpp

	${CMAKE_SOURCE_DIR}/editors/scapp/CocoaBridgePrimitives.M
	${CMAKE_SOURCE_DIR}/editors/scapp/CocoaFilePrimitives.M
	${CMAKE_SOURCE_DIR}/editors/scapp/DrawBackground.M
	${CMAKE_SOURCE_DIR}/editors/scapp/GUIPrimitives.M
	${CMAKE_SOURCE_DIR}/editors/scapp/GetStringFromUser.m
	${CMAKE_SOURCE_DIR}/editors/scapp/GoToPanel.M
	${CMAKE_SOURCE_DIR}/editors/scapp/HTMLRenderer.m
	${CMAKE_SOURCE_DIR}/editors/scapp/MyDocument.M
	${CMAKE_SOURCE_DIR}/editors/scapp/RendezvousClient.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCCocoaView.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCDialog.mm
	${CMAKE_SOURCE_DIR}/editors/scapp/SCGraphView.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCImage.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCNSWindow.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCService.mm
	${CMAKE_SOURCE_DIR}/editors/scapp/SCSoundFileView.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCTextView.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCView.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCVirtualMachine.M
	${CMAKE_SOURCE_DIR}/editors/scapp/TabletEvents.m
	${CMAKE_SOURCE_DIR}/editors/scapp/TextFinder.m
	${CMAKE_SOURCE_DIR}/editors/scapp/UserPanel.M
	${CMAKE_SOURCE_DIR}/editors/scapp/main.M
	${CMAKE_SOURCE_DIR}/editors/scapp/post_queue.M
	${CMAKE_SOURCE_DIR}/editors/scapp/syntaxColorize.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SMLAdvancedFind/SMLAdvancedFindController.m
	${CMAKE_SOURCE_DIR}/editors/scapp/SMLAdvancedFind/SMLStatusBarTextFieldCell.m
	${CMAKE_SOURCE_DIR}/editors/scapp/SMLAdvancedFind/ICU/ICUMatcher.m
	${CMAKE_SOURCE_DIR}/editors/scapp/SMLAdvancedFind/ICU/ICUPattern.m
	${CMAKE_SOURCE_DIR}/editors/scapp/SMLAdvancedFind/ICU/NSStringICUAdditions.m
	)

file(GLOB boost_system_sources ${CMAKE_SOURCE_DIR}/external_libraries/boost/libs/system/src/*cpp)
file(GLOB boost_fs_sources ${CMAKE_SOURCE_DIR}/external_libraries/boost/libs/filesystem/src/*cpp)
file(GLOB boost_regex_sources ${CMAKE_SOURCE_DIR}/external_libraries/boost/libs/regex/src/*cpp)
list(APPEND scapp_sources ${boost_fs_sources} ${boost_system_sources} ${boost_regex_sources})

if(SC_QT)
	include(${CMAKE_SOURCE_DIR}/QtCollider/CMakeLists.txt)
	list(APPEND scapp_sources ${QT_COLLIDER_SRCS})
endif()

include(${CMAKE_SOURCE_DIR}/SCDoc/CMakeLists.txt)
list(APPEND scapp_sources ${SCDOC_SRCS})

set(objcpp_sources
	${CMAKE_SOURCE_DIR}/lang/LangPrimSource/SC_Speech.M
	${CMAKE_SOURCE_DIR}/editors/scapp/main.M
	${CMAKE_SOURCE_DIR}/editors/scapp/CocoaBridgePrimitives.M
	${CMAKE_SOURCE_DIR}/editors/scapp/CocoaFilePrimitives.M
	${CMAKE_SOURCE_DIR}/editors/scapp/DrawBackground.M
	${CMAKE_SOURCE_DIR}/editors/scapp/GUIPrimitives.M
	${CMAKE_SOURCE_DIR}/editors/scapp/GoToPanel.M
	${CMAKE_SOURCE_DIR}/editors/scapp/MyDocument.M
	${CMAKE_SOURCE_DIR}/editors/scapp/RendezvousClient.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCCocoaView.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCNSWindow.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCGraphView.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCImage.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCService.mm
	${CMAKE_SOURCE_DIR}/editors/scapp/SCSoundFileView.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCTextView.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCView.M
	${CMAKE_SOURCE_DIR}/editors/scapp/SCVirtualMachine.M
	${CMAKE_SOURCE_DIR}/editors/scapp/UserPanel.M
	${CMAKE_SOURCE_DIR}/editors/scapp/post_queue.M
	${CMAKE_SOURCE_DIR}/editors/scapp/syntaxColorize.M
	)

set(objc_sources
	${CMAKE_SOURCE_DIR}/editors/scapp/GetStringFromUser.m
	${CMAKE_SOURCE_DIR}/editors/scapp/HTMLRenderer.m
	${CMAKE_SOURCE_DIR}/editors/scapp/TabletEvents.m
	${CMAKE_SOURCE_DIR}/editors/scapp/TextFinder.m
	${CMAKE_SOURCE_DIR}/editors/scapp/SMLAdvancedFind/SMLAdvancedFindController.m
	${CMAKE_SOURCE_DIR}/editors/scapp/SMLAdvancedFind/SMLStatusBarTextFieldCell.m
	${CMAKE_SOURCE_DIR}/editors/scapp/SMLAdvancedFind/ICU/ICUMatcher.m
	${CMAKE_SOURCE_DIR}/editors/scapp/SMLAdvancedFind/ICU/ICUPattern.m
	${CMAKE_SOURCE_DIR}/editors/scapp/SMLAdvancedFind/ICU/NSStringICUAdditions.m
)

if (CMAKE_COMPILER_IS_GNUCXX)
	set_source_files_properties(${objcpp_sources}
								PROPERTIES COMPILE_FLAGS "-x objective-c++ -fobjc-exceptions")
	set_source_files_properties(${objc_sources}
								PROPERTIES COMPILE_FLAGS "-fobjc-exceptions")
endif()

if(NOT (HID_UTIL_DIR MATCHES "HID_Utilities_10_4"))
	list(APPEND scapp_sources
		"${CMAKE_SOURCE_DIR}/lang/LangPrimSource/${HID_UTIL_DIR}/IOHIDElement_.c"
		"${CMAKE_SOURCE_DIR}/lang/LangPrimSource/${HID_UTIL_DIR}/IOHIDDevice_.c"
	)
endif()

set(scapp_parser_source ${CMAKE_SOURCE_DIR}/lang/LangSource/Bison/lang11d_tab.cpp)

add_definitions(-DYYSTACK_USE_ALLOCA -DHAVE_SPEECH)

# set how it shows up in the Info.plist file
SET(MACOSX_BUNDLE_ICON_FILE SCcube.icns)

install(DIRECTORY Resources/ DESTINATION ${scappauxresourcesdir})

add_executable(${scappbundlename} MACOSX_BUNDLE
	${scapp_sources} ${scapp_parser_source}
	)

if (SCLANG_SERVER)
	target_link_libraries(${scappbundlename} libscsynth)
else()
	set_property(TARGET ${scappbundlename}
		APPEND
		PROPERTY COMPILE_DEFINITIONS NO_INTERNAL_SERVER
		)
endif()

if(SC_QT)
	target_link_libraries(${scappbundlename} ${QT_COLLIDER_LIBS})
endif()

# for processing into the plist:
set_property(TARGET ${scappbundlename}
	PROPERTY MACOSX_BUNDLE_BUNDLE_NAME ${scappbundlename})
# also TODO, separate gui name? (currently using MACOSX_BUNDLE_BUNDLE_NAME twice)

target_link_libraries(${scappbundlename} icucore)
if (Boost_FOUND)
	target_link_libraries(${scappbundlename} ${Boost_THREAD_LIBRARY})
else()
	target_link_libraries(${scappbundlename} boost_thread)
endif()
target_link_libraries(${scappbundlename} ${YAMLCPP_LIBRARY})

target_link_libraries(${scappbundlename} "-framework ApplicationServices")
target_link_libraries(${scappbundlename} "-framework Carbon")
target_link_libraries(${scappbundlename} "-framework Cocoa")
target_link_libraries(${scappbundlename} "-framework CoreAudio")
target_link_libraries(${scappbundlename} "-framework CoreMIDI")
target_link_libraries(${scappbundlename} "-framework CoreServices")
target_link_libraries(${scappbundlename} "-framework IOBluetooth")
target_link_libraries(${scappbundlename} "-framework IOKit")
target_link_libraries(${scappbundlename} "-framework Quartz")
target_link_libraries(${scappbundlename} "-framework QuartzCore")
target_link_libraries(${scappbundlename} "-framework QuickTime")
target_link_libraries(${scappbundlename} "-framework QTKit")
target_link_libraries(${scappbundlename} "-framework WebKit")

if(SNDFILE_FOUND)
	include_directories(${SNDFILE_INCLUDE_DIR})
	target_link_libraries(${scappbundlename} ${SNDFILE_LIBRARIES})
	add_definitions("-DLIBSNDFILE_1018")
elseif(NOT NO_LIBSNDFILE)
	message(SEND_ERROR "Cannot find libsndfile")
endif(SNDFILE_FOUND)

target_link_libraries(${scappbundlename} ${PTHREADS_LIBRARY})

install(TARGETS ${scappbundlename} DESTINATION ${scappbundlename})

if (NOT ("${standalone}" STREQUAL ""))
	# Standalones are also given a chance to overwrite things from their specially-manufactured resources dir.
	# If "${scappbundlename} Resources" exists then that's used; otherwise it uses "Standalone Resources".
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${scappbundlename} Resources")
		set(standaloneresourcesfrom "${scappbundlename} Resources")
	else()
		set(standaloneresourcesfrom "Standalone Resources")
	endif()
	message(STATUS "Including standalone resources from folder named: " ${standaloneresourcesfrom})
    install(DIRECTORY ${standaloneresourcesfrom}/ DESTINATION ${scappauxresourcesdir})
    add_definitions(-DSC_STANDALONE)
endif()

set(MACOSX_BUNDLE_BUNDLE_NAME "${scappbundlename}")

include(InstallRequiredSystemLibraries)

include(DeployQt4)
install_qt4_executable("${scappbundlename}/${scappbundlename}.app" "" "" "${CMAKE_BINARY_DIR}/server/scsynth;${CMAKE_BINARY_DIR}/external_libraries")
