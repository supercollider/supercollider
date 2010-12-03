if(NOT APPLE)
		message(SEND_ERROR "mac folder invoked by cmake, but not APPLE platform")
endif()

# determines the app name and app install location (scappbundlename, scappdir):
include (../common/cmake_modules/MacAppFolder.cmake)

include_directories(../common/Headers/lang
	../common/Source/lang/LangSource/Bison
	../common/Headers/common
	../common/Headers/plugin_interface
	../common/Headers/app
	../common/Headers/app/SMLAdvancedFind
	../common/Headers/app/SMLAdvancedFind/ICU
	../common/include/icu/unicode/
	)

include_directories(../common/include/icu
	../common/Source/lang/LangPrimSource/HID_Utilities)

find_package(Sndfile)

set(scapp_sources
	../common/Source/lang/LangPrimSource/SC_Wii.cpp
	../common/Source/lang/LangPrimSource/PyrSignalPrim.cpp
	../common/Source/lang/LangPrimSource/PyrSched.cpp
	../common/Source/lang/LangPrimSource/PyrPrimitive.cpp
	../common/Source/lang/LangPrimSource/PyrMathPrim.cpp
	../common/Source/lang/LangPrimSource/SC_ComPort.cpp
	../common/Source/lang/LangPrimSource/OSCData.cpp
	../common/Source/lang/LangPrimSource/PyrArchiver.cpp
	../common/Source/lang/LangPrimSource/PyrArrayPrimitives.cpp
	../common/Source/lang/LangPrimSource/PyrBitPrim.cpp
	../common/Source/lang/LangPrimSource/PyrCharPrim.cpp
	../common/Source/lang/LangPrimSource/PyrFilePrim.cpp
	../common/Source/lang/LangPrimSource/PyrListPrim.cpp
	../common/Source/lang/LangPrimSource/PyrPlatformPrim.cpp
	../common/Source/lang/LangPrimSource/PyrSerialPrim.cpp
	../common/Source/lang/LangPrimSource/PyrStringPrim.cpp
	../common/Source/lang/LangPrimSource/PyrUStringPrim.cpp
	../common/Source/lang/LangPrimSource/PyrSymbolPrim.cpp
	../common/Source/lang/LangPrimSource/PyrUnixPrim.cpp
	../common/Source/lang/LangPrimSource/SC_Speech.M
	../common/Source/lang/LangSource/AdvancingAllocPool.cpp
	../common/Source/lang/LangSource/ByteCodeArray.cpp
	../common/Source/lang/LangSource/DumpParseNode.cpp
	../common/Source/lang/LangSource/GC.cpp
	../common/Source/lang/LangSource/InitAlloc.cpp
	../common/Source/lang/LangSource/PyrFileUtils.cpp
	../common/Source/lang/LangSource/PyrInterpreter3.cpp
	../common/Source/lang/LangSource/PyrLexer.cpp
	../common/Source/lang/LangSource/PyrMathOps.cpp
	../common/Source/lang/LangSource/PyrMathSupport.cpp
	../common/Source/lang/LangSource/PyrMessage.cpp
	../common/Source/lang/LangSource/PyrObject.cpp
	../common/Source/lang/LangSource/PyrParseNode.cpp
	../common/Source/lang/LangSource/PyrSignal.cpp
	../common/Source/lang/LangSource/PyrSymbolTable.cpp
	../common/Source/lang/LangSource/SC_LibraryConfig.cpp
	../common/Source/lang/LangSource/Samp.cpp
	../common/Source/lang/LangSource/SimpleStack.cpp
	../common/Source/lang/LangSource/VMGlobals.cpp
	../common/Source/lang/LangSource/dumpByteCodes.cpp
	../common/Source/lang/LangPrimSource/WiiMote_OSX/wiiremote.c
	../common/Source/lang/LangPrimSource/SC_HID.cpp
	../common/Source/lang/LangPrimSource/HID_Utilities/IOHIDElement_.c
	../common/Source/lang/LangPrimSource/HID_Utilities/IOHIDDevice_.c
	../common/Source/lang/LangPrimSource/HID_Utilities/HID_Utilities.c
	../common/Source/lang/LangPrimSource/HID_Utilities/HID_Name_Lookup.c
	../common/Source/lang/LangPrimSource/HID_Utilities/HID_Queue_Utilities.c
	../common/Source/lang/LangPrimSource/HID_Utilities/HID_Error_Handler.c
	../common/Source/lang/LangPrimSource/SC_CoreMIDI.cpp
	../common/Source/lang/LangPrimSource/SC_CoreAudioPrim.cpp
	../common/Source/common/fftlib.c
	../common/Source/app/CocoaBridgePrimitives.M
	../common/Source/app/CocoaFilePrimitives.M
	../common/Source/app/DrawBackground.M
	../common/Source/app/GUIPrimitives.M
	../common/Source/app/GetStringFromUser.m
	../common/Source/app/GoToPanel.M
	../common/Source/app/HTMLRenderer.m
	../common/Source/app/MyDocument.M
	../common/Source/app/RendezvousClient.M
	../common/Source/app/SCCocoaView.M
	../common/Source/app/SCDialog.mm
	../common/Source/app/SCGraphView.M
	../common/Source/app/SCImage.M
	../common/Source/app/SCNSWindow.m
	../common/Source/app/SCService.mm
	../common/Source/app/SCSoundFileView.M
	../common/Source/app/SCTextView.M
	../common/Source/app/SCView.M
	../common/Source/app/SCVirtualMachine.M
	../common/Source/app/TabletEvents.m
	../common/Source/app/TextFinder.m
	../common/Source/app/UserPanel.M
	../common/Source/app/main.m
	../common/Source/app/post_queue.M
	../common/Source/app/syntaxColorize.M
	../common/Source/app/SMLAdvancedFind/SMLAdvancedFindController.m
	../common/Source/app/SMLAdvancedFind/SMLStatusBarTextFieldCell.m
	../common/Source/app/SMLAdvancedFind/ICU/ICUMatcher.m
	../common/Source/app/SMLAdvancedFind/ICU/ICUPattern.m
	../common/Source/app/SMLAdvancedFind/ICU/NSStringICUAdditions.m
	)

set(scapp_parser_source ../common/Source/lang/LangSource/Bison/lang11d_tab.cpp)

add_definitions(-DYYSTACK_USE_ALLOCA -DHAVE_SPEECH)

# set how it shows up in the Info.plist file
SET(MACOSX_BUNDLE_ICON_FILE SCcube.icns) 
# set where in the bundle to put the icns file
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/Resources/SCcube.icns
	PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
# include the icns file in the target
SET(scapp_resources ${CMAKE_CURRENT_SOURCE_DIR}/Resources/SCcube.icns)

file(COPY Resources
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${scappbundlename}.app/Contents
	PATTERN Info.plist EXCLUDE
	PATTERN .svn EXCLUDE
	PATTERN .git EXCLUDE
)

add_executable(${scappbundlename} MACOSX_BUNDLE
	${scapp_sources} ${scapp_parser_source}
	${scapp_resources}
	)

if (SCLANG_SERVER)
	target_link_libraries(${scappbundlename} libscsynth)
else()
	set_property(TARGET ${scappbundlename}
		APPEND
		PROPERTY COMPILE_DEFINITIONS NO_INTERNAL_SERVER
		)
endif()

# for processing into the plist:
set_property(TARGET ${scappbundlename}
	PROPERTY MACOSX_BUNDLE_BUNDLE_NAME ${scappbundlename})
# also TODO, separate gui name? (currently using MACOSX_BUNDLE_BUNDLE_NAME twice)

target_link_libraries(${scappbundlename} icucore)

target_link_libraries(${scappbundlename} "-framework ApplicationServices")
target_link_libraries(${scappbundlename} "-framework Carbon")
target_link_libraries(${scappbundlename} "-framework Cocoa")
target_link_libraries(${scappbundlename} "-framework CoreAudio")
target_link_libraries(${scappbundlename} "-framework CoreMidi")
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

target_link_libraries(${scappbundlename} sccommon pthread)

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${scappbundlename}.app"
		DESTINATION ${scappdir}
		USE_SOURCE_PERMISSIONS
		)

if (NOT ("${standalone}" STREQUAL ""))
	# A synthdefs folder is expected in the bundle
	file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${scappbundlename}.app/Contents/Resources/synthdefs")

	# TODO: Standalones are also given a chance to overwrite things from their specially-manufactured resources dir
endif()
