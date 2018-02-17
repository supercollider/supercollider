if(NOT NO_LIBSNDFILE)
	find_package(Sndfile)
else()
	add_definitions("-DNO_LIBSNDFILE")
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
if(APPLE OR WIN32)
	set(CMAKE_SHARED_MODULE_SUFFIX ".scx")
endif()

if (${CMAKE_COMPILER_IS_GNUCXX})
	add_definitions(-fno-finite-math-only)
	if(NOT WIN32)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
	endif()
endif()

foreach(plugin ${plugin_sources})
	string(REPLACE .cpp "" plugin_name ${plugin} )
		add_library(${plugin_name} MODULE ${plugin})
	list(APPEND plugins ${plugin_name})
endforeach(plugin)


if (APPLE)
	add_library(UIUGens MODULE UIUGens.mm)
else()
	add_library(UIUGens MODULE UIUGens.cpp)
	target_link_libraries(UIUGens ${PTHREADS_LIBRARY})
endif()

set(uiugens UIUGens)

list(APPEND plugins UIUGens)

if (WIN32)
	if(MSVC)
		include_directories (../../platform/windows/compat_stuff) # for stdbool.h
	endif()
endif()

add_library(FFT_UGens MODULE
	FFTInterfaceTable.cpp
	FFT_UGens.cpp
	PV_UGens.cpp
	PartitionedConvolution.cpp
	${scplugin_common_sources}
)

add_library(PV_ThirdParty MODULE
	Convolution.cpp
	FFT2InterfaceTable.cpp
	FeatureDetection.cpp
	PV_ThirdParty.cpp
	${scplugin_common_sources}
)

add_library(ML_UGens MODULE
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
	${scplugin_common_sources}
)

if(NOT NO_LIBSNDFILE)
	set(diskio_sources DiskIO_UGens.cpp)

	add_library(DiskIO_UGens MODULE ${diskio_sources})

	if(SNDFILE_FOUND)
		target_include_directories(DiskIO_UGens PUBLIC ${SNDFILE_INCLUDE_DIR})
		target_link_libraries(DiskIO_UGens ${PTHREADS_LIBRARY} ${SNDFILE_LIBRARIES})
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
	list(APPEND supernova_plugins FFT_UGens_supernova PV_ThirdParty_supernova ML_UGens_supernova UIUGens_supernova)

	if (APPLE)
		add_library(UIUGens_supernova MODULE UIUGens.mm)
	else()
		add_library(UIUGens_supernova MODULE UIUGens.cpp)
		target_link_libraries(UIUGens_supernova ${PTHREADS_LIBRARY})
	endif()
	list(APPEND uiugens UIUGens_supernova)

	if(NOT NO_LIBSNDFILE)
		add_library(DiskIO_UGens_supernova MODULE DiskIO_UGens.cpp)

		if(SNDFILE_FOUND)
			target_include_directories(DiskIO_UGens_supernova PUBLIC ${SNDFILE_INCLUDE_DIR})
			target_link_libraries(DiskIO_UGens_supernova ${SNDFILE_LIBRARIES} ${PTHREADS_LIBRARY} ${MATH_LIBRARY} ${Boost_THREAD_LIBRARY})
			list(APPEND supernova_plugins DiskIO_UGens_supernova)
		else()
			message(SEND_ERROR "Cannot find libsndfile")
		endif(SNDFILE_FOUND)
	endif()
endif(SUPERNOVA)

foreach(ugen ${fft_ugens})
	target_link_libraries(${ugen} ${PTHREADS_LIBRARY} ${MATH_LIBRARY})

	if(APPLE)
		target_link_libraries(${ugen} "-framework Accelerate")
	endif()
endforeach()

foreach(ugen ${uiugens})
	target_link_libraries(${ugen} ${Boost_THREAD_LIBRARY})
endforeach()

foreach(plugin ${plugins})
	if(WIN32)
		target_link_libraries(${plugin} wsock32 ws2_32)
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
		target_link_libraries(${plugin} ws2_32)
	endif()
endforeach()

if (CMAKE_SYSTEM_NAME MATCHES "Linux|FreeBSD|DragonFly|OpenBSD|NetBSD")
	find_package(X11 REQUIRED)

	if (NOT X11_Xt_FOUND)
		message(FATAL_ERROR "libxt not found")
	endif()

	include_directories(${X11_Xt_INCLUDE_PATH})

	foreach (ugen ${uiugens})
		target_link_libraries(${ugen} ${X11_X11_LIB})
	endforeach()
elseif (APPLE)
	foreach (ugen ${uiugens})
		target_link_libraries(${ugen} "-framework CoreServices -framework AppKit")
	endforeach()

	if(NOT NO_LIBSNDFILE)
		target_link_libraries(DiskIO_UGens "-framework CoreServices")
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
	foreach(plugin ${plugins} ${supernova_plugins})
		add_custom_command(TARGET ${plugin} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:SuperCollider>/../Resources/plugins/
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${plugin}> $<TARGET_FILE_DIR:SuperCollider>/../Resources/plugins/)
	endforeach()

else()
	install(TARGETS ${plugins} ${supernova_plugins}
			DESTINATION "lib${LIB_SUFFIX}/SuperCollider/plugins"
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

set_property(TARGET ${plugins} ${supernova_plugins} PROPERTY FOLDER UnitGenerators)

set(plugins "${plugins}" PARENT_SCOPE)
set(supernova_plugins "${supernova_plugins}" PARENT_SCOPE)
