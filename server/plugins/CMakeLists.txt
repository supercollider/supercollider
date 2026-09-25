if(NOT NO_LIBSNDFILE)
	find_package(Sndfile)
endif()

include_directories(
	${CMAKE_SOURCE_DIR}/include/common
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/include/plugin_interface
)

# large file support
if (CMAKE_SIZEOF_VOID_P MATCHES "4")
	execute_process(COMMAND getconf LFS_CFLAGS
		OUTPUT_VARIABLE LFS_CFLAGS
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)
elseif(CMAKE_SIZEOF_VOID_P MATCHES "8")
	execute_process(COMMAND getconf LFS64_CFLAGS
		OUTPUT_VARIABLE LFS_CFLAGS
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LFS_CFLAGS}")

set(plugin_sources
	BinaryOpUGens.cpp
	ChaosUGens.cpp
	DelayUGens.cpp
	DemandUGens.cpp
	DemoUGens.cpp
	DynNoiseUGens.cpp
	FilterUGens.cpp
	GendynUGens.cpp
	GrainUGens.cpp
	IOUGens.cpp
	LFUGens.cpp
	MulAddUGens.cpp
	NoiseUGens.cpp
	OscUGens.cpp
	PanUGens.cpp
	PhysicalModelingUGens.cpp
	ReverbUGens.cpp
	TestUGens.cpp
	TriggerUGens.cpp
	UnaryOpUGens.cpp
	UnpackFFTUGens.cpp
)

set(plugins "")
set(supernova_plugins "")

set(CMAKE_SHARED_MODULE_PREFIX "")
set(CMAKE_SHARED_MODULE_SUFFIX ".scx")

foreach(plugin ${plugin_sources})
	string(REPLACE .cpp "" plugin_name ${plugin} )
	add_library(${plugin_name} MODULE ${plugin})
	target_link_libraries(${plugin_name} PRIVATE Boost::headers)
	list(APPEND plugins ${plugin_name})
endforeach(plugin)

if(AUDIOAPI STREQUAL bela)
    # The Bela lib will have its own set of libraries and includes
    # However, the Bela audio backend for SC has some direct calls to Xenomai
    # Therefore we need to get flags for those as well
    find_package(Xenomai REQUIRED)
    find_package(Bela REQUIRED)
    message(STATUS "Found Bela libraries: ${BELA_LIBRARIES}")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # recommended compile flags for beaglebone etc; set here because bela api flag directly implies the architecture
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${BELA_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BELA_CXX_FLAGS}")
    endif()
endif()

if(BELA_FOUND)
    add_library(BelaUGens MODULE
        BelaUGens.cpp
    )
    target_compile_definitions(BelaUGens PRIVATE SC_BELA)
	target_compile_options(BelaUGens PRIVATE ${XENOMAI_DEFINITIONS} ${BELA_DEFINITIONS})
    include_directories(${XENOMAI_INCLUDE_DIRS} ${BELA_INCLUDE_DIRS})
    target_link_libraries(BelaUGens PRIVATE ${XENOMAI_LIBRARIES} ${BELA_LIBRARIES})

    list(APPEND plugins BelaUGens)
endif()

if(NOT NO_X11)
	if (APPLE)
		set(ui_sources UIUGens.mm)
		add_library(UIUGens MODULE ${ui_sources})
	else()
		set(ui_sources UIUGens.cpp)
		add_library(UIUGens MODULE ${ui_sources})
		target_link_libraries(UIUGens PRIVATE ${PTHREADS_LIBRARY})
	endif()

	set(uiugens UIUGens)

	list(APPEND plugins UIUGens)
endif()

set(FFT_sources
	FFTInterfaceTable.cpp
	FFT_UGens.cpp
	PV_UGens.cpp
	PartitionedConvolution.cpp
)

add_library(FFT_UGens MODULE
	${FFT_sources}
	${scplugin_common_sources}
)

set(PV_ThirdParty_sources
	Convolution.cpp
	FFT2InterfaceTable.cpp
	FeatureDetection.cpp
	PV_ThirdParty.cpp
)

add_library(PV_ThirdParty MODULE
	${PV_ThirdParty_sources}
	${scplugin_common_sources}
)

set(ML_sources
	ML.cpp
	Loudness.cpp
	BeatTrack.cpp
	Onsets.cpp
	onsetsds.c
	KeyTrack.cpp
	MFCC.cpp
	BeatTrack2.cpp
	ML_SpecStats.cpp
	BeatTrack.h
	BeatTrack2.h
	ML.h
	Onsets.h
	onsetsds.h
)

add_library(ML_UGens MODULE
	${ML_sources}
	${scplugin_common_sources}
)

if(SC_ABLETON_LINK)
	add_library(Link_UGen MODULE
		LinkUGen.cpp
	)
	# bug-fix for undefined reference to `GetAdaptersAddresses'
	if (MINGW)
		target_link_libraries(Link_UGen PRIVATE iphlpapi)
	endif()
	include(../../external_libraries/link/AbletonLinkConfig.cmake)
	target_link_libraries(Link_UGen PRIVATE Ableton::Link)
	list(APPEND plugins Link_UGen)
endif()

if(NOT NO_LIBSNDFILE)
	set(diskio_sources DiskIO_UGens.cpp)

	if (XENOMAI_FOUND)
		list(APPEND diskio_sources ../../common/XenomaiLock.cpp)
	endif()
	add_library(DiskIO_UGens MODULE ${diskio_sources})

	if(SNDFILE_FOUND)
		if(SNDFILE_HAS_VORBIS)
			target_compile_definitions(DiskIO_UGens PRIVATE SNDFILE_HAS_VORBIS)
		endif()
		if(SNDFILE_HAS_OPUS)
			target_compile_definitions(DiskIO_UGens PRIVATE SNDFILE_HAS_OPUS)
		endif()
		if(SNDFILE_HAS_MPEG)
			target_compile_definitions(DiskIO_UGens PRIVATE SNDFILE_HAS_MPEG)
		endif()
		target_include_directories(DiskIO_UGens SYSTEM PRIVATE ${SNDFILE_INCLUDE_DIR})
		target_link_libraries(DiskIO_UGens PRIVATE ${PTHREADS_LIBRARY} ${SNDFILE_LIBRARIES})
		list(APPEND plugins DiskIO_UGens)
	else()
		message(SEND_ERROR "Cannot find libsndfile")
	endif(SNDFILE_FOUND)
endif()

set(fft_ugens FFT_UGens PV_ThirdParty ML_UGens)
list(APPEND plugins FFT_UGens PV_ThirdParty ML_UGens)

if (FFTW3F_FOUND)
	include_directories (${FFTW3F_INCLUDE_DIR})
endif()

if (SUPERNOVA)
	foreach(plugin ${plugin_sources})
		string(REPLACE .cpp "_supernova" plugin_name ${plugin} )
		add_library(${plugin_name} MODULE ${plugin})
		target_link_libraries(${plugin_name} PRIVATE Boost::headers)
		list(APPEND supernova_plugins ${plugin_name})
	endforeach(plugin)

	add_library(FFT_UGens_supernova MODULE
		FFTInterfaceTable.cpp
		FFT_UGens.cpp
		PV_UGens.cpp
		PartitionedConvolution.cpp
		${scplugin_common_sources}
	)

	add_library(PV_ThirdParty_supernova MODULE
		Convolution.cpp
		FFT2InterfaceTable.cpp
		FeatureDetection.cpp
		PV_ThirdParty.cpp
		${scplugin_common_sources}
	)

	add_library(ML_UGens_supernova MODULE
		ML.cpp
		Loudness.cpp
		BeatTrack.cpp
		Onsets.cpp
		onsetsds.c
		KeyTrack.cpp
		MFCC.cpp
		BeatTrack2.cpp
		ML_SpecStats.cpp
		${scplugin_common_sources}
	)

	list(APPEND fft_ugens FFT_UGens_supernova PV_ThirdParty_supernova ML_UGens_supernova)
	list(APPEND supernova_plugins FFT_UGens_supernova PV_ThirdParty_supernova ML_UGens_supernova)

	if(NOT NO_X11)
		list(APPEND supernova_plugins UIUGens_supernova)

		if (APPLE)
			add_library(UIUGens_supernova MODULE UIUGens.mm)
		else()
			add_library(UIUGens_supernova MODULE UIUGens.cpp)
			target_link_libraries(UIUGens_supernova PRIVATE ${PTHREADS_LIBRARY})
		endif()
		list(APPEND uiugens UIUGens_supernova)
	endif()

	if(NOT NO_LIBSNDFILE)
		add_library(DiskIO_UGens_supernova MODULE DiskIO_UGens.cpp)

		if(SNDFILE_FOUND)
			target_include_directories(DiskIO_UGens_supernova SYSTEM PUBLIC ${SNDFILE_INCLUDE_DIR})
      target_link_libraries(DiskIO_UGens_supernova PRIVATE ${SNDFILE_LIBRARIES} ${PTHREADS_LIBRARY} ${MATH_LIBRARY} Boost::thread)
			list(APPEND supernova_plugins DiskIO_UGens_supernova)
		else()
			message(SEND_ERROR "Cannot find libsndfile")
		endif(SNDFILE_FOUND)
	endif()
endif(SUPERNOVA)

foreach(ugen ${fft_ugens})
	target_link_libraries(${ugen} PRIVATE ${PTHREADS_LIBRARY} ${MATH_LIBRARY})

	if(APPLE)
		target_link_libraries(${ugen} PRIVATE "-framework Accelerate")
	endif()
endforeach()

if(NOT NO_X11)
	foreach(ugen ${uiugens})
    target_link_libraries(${ugen} PRIVATE Boost::thread)
	endforeach()
endif()

foreach(plugin ${plugins})
	target_link_libraries(${plugin} PRIVATE Boost::headers)
	if(WIN32)
		target_link_libraries(${plugin} PRIVATE wsock32 ws2_32)
	endif()
endforeach()


if(NOVA_SIMD)
	foreach(plugin ${plugins} ${supernova_plugins})
		target_include_directories(${plugin} PUBLIC ${CMAKE_SOURCE_DIR}/external_libraries/nova-simd)
		target_compile_definitions(${plugin} PUBLIC NOVA_SIMD)
	endforeach()
endif()

foreach(plugin ${supernova_plugins})
	target_compile_definitions(${plugin} PUBLIC SUPERNOVA)

	if (WIN32)
		target_link_libraries(${plugin} PRIVATE ws2_32)
	endif()
endforeach()

if(NOT NO_X11)
	if (LINUX_OR_BSD)
		find_package(X11 REQUIRED)

		if (NOT X11_Xt_FOUND)
			message(FATAL_ERROR "libxt not found")
		endif()

		include_directories(${X11_Xt_INCLUDE_PATH})

		foreach (ugen ${uiugens})
			target_link_libraries(${ugen} PRIVATE ${X11_X11_LIB})
		endforeach()
	elseif (APPLE)
		foreach (ugen ${uiugens})
			target_link_libraries(${ugen} PRIVATE "-framework CoreServices -framework AppKit")
		endforeach()
	endif()
endif()

if (APPLE)
	if(NOT NO_LIBSNDFILE)
		target_link_libraries(DiskIO_UGens PRIVATE "-framework CoreServices")
	endif()
endif()

if (WIN32)
	foreach(plugin ${plugins} ${supernova_plugins})
		add_custom_command(TARGET ${plugin}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:scsynth>/plugins
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${plugin}> $<TARGET_FILE_DIR:scsynth>/plugins
			COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:sclang>/plugins
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${plugin}> $<TARGET_FILE_DIR:sclang>/plugins
		)
	endforeach()

	if(SC_IDE)
		foreach(plugin ${plugins} ${supernova_plugins})
			add_custom_command(TARGET ${plugin}
				POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:SuperCollider>/plugins
				COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${plugin}> $<TARGET_FILE_DIR:SuperCollider>/plugins
			)
		endforeach()
	endif(SC_IDE)

	if(SUPERNOVA)
		foreach(plugin ${supernova_plugins})
			add_custom_command(TARGET ${plugin}
				POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:supernova>/plugins
				COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${plugin}> $<TARGET_FILE_DIR:supernova>/plugins
			)
		endforeach()
	endif(SUPERNOVA)

	install(TARGETS ${plugins} ${supernova_plugins}
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
		DESTINATION "${SC_WIN_BUNDLE_NAME}/plugins"
	)

elseif(APPLE)
	if(scappauxresourcesdir)
		foreach(plugin ${plugins} ${supernova_plugins})
			add_custom_command(TARGET ${plugin} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/${scappauxresourcesdir}/plugins
				COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${plugin}> ${CMAKE_INSTALL_PREFIX}/${scappauxresourcesdir}/plugins)
		endforeach()
	endif()

else()
	install(TARGETS ${plugins} ${supernova_plugins}
			DESTINATION "lib${LIB_SUFFIX}/SuperCollider/plugins"
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

set_property(TARGET ${plugins} ${supernova_plugins} PROPERTY FOLDER UnitGenerators)

set(plugins "${plugins}" PARENT_SCOPE)
set(supernova_plugins "${supernova_plugins}" PARENT_SCOPE)

if(STATIC_PLUGINS)
	set(all_plugin_sources
		${plugin_sources}
		${FFT_sources}
		${PV_ThirdParty_sources}
		${ML_sources}
		${diskio_sources}
		${ui_sources}
	PARENT_SCOPE)
endif()
