
if(NOT NO_LIBSNDFILE)
	find_package(Sndfile)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include/common
                    ${CMAKE_SOURCE_DIR}/include/plugin_interface)

if (NOVA_SIMD)
	include_directories(${CMAKE_SOURCE_DIR}/external_libraries/nova-simd)
	add_definitions("-DNOVA_SIMD")
endif()

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

	KeyboardUGens.cpp
	MouseUGens.cpp
)

set(plugins "")
set(supernova_plugins "")

set(CMAKE_SHARED_MODULE_PREFIX "")
if(APPLE OR WIN32)
	set(CMAKE_SHARED_MODULE_SUFFIX ".scx")
endif()

if (${CMAKE_COMPILER_IS_GNUCXX})
	add_definitions(-fno-finite-math-only)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions -fvisibility-inlines-hidden")
endif()

foreach(plugin ${plugin_sources})
	string(REPLACE .cpp "" plugin_name ${plugin} )
        if(WIN32)
        	add_library(${plugin_name} MODULE ${plugin} ../../platform/windows/PlugIns/ExportHelper.cpp)
        else()
        	add_library(${plugin_name} MODULE ${plugin})
        endif()
	list(APPEND plugins ${plugin_name})
endforeach(plugin)

set(uiugens MouseUGens KeyboardUGens)

if (WIN32)
    set(scplugin_common_sources ../../platform/windows/PlugIns/ExportHelper.cpp)
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
			${scplugin_common_sources}
			)

if(NOT NO_LIBSNDFILE)
	set(diskio_sources DiskIO_UGens.cpp)

	if(WIN32)
		set(diskio_sources ${diskio_sources} ../../platform/windows/PlugIns/ExportHelper.cpp)
	endif()

	add_library(DiskIO_UGens MODULE ${diskio_sources})

	if(SNDFILE_FOUND)
		include_directories(${SNDFILE_INCLUDE_DIR})
		target_link_libraries(DiskIO_UGens ${SNDFILE_LIBRARIES} ${MATH_LIBRARY} ${PTHREADS_LIBRARY})

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
		set_property(TARGET ${plugin_name}
                    APPEND PROPERTY COMPILE_DEFINITIONS  "SUPERNOVA")
	endforeach(plugin)

	list(APPEND uiugens KeyboardUGens_supernova MouseUGens_supernova)

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

	set_property(TARGET FFT_UGens_supernova PV_ThirdParty_supernova ML_UGens_supernova
				APPEND PROPERTY COMPILE_DEFINITIONS  "SUPERNOVA")

	list(APPEND fft_ugens FFT_UGens_supernova PV_ThirdParty_supernova ML_UGens_supernova)
	list(APPEND supernova_plugins FFT_UGens_supernova PV_ThirdParty_supernova ML_UGens_supernova)

	if(NOT NO_LIBSNDFILE)
		add_library(DiskIO_UGens_supernova MODULE
				DiskIO_UGens.cpp)
		set_property(TARGET DiskIO_UGens_supernova
					APPEND PROPERTY COMPILE_DEFINITIONS  "SUPERNOVA")

		if(SNDFILE_FOUND)
			include_directories(${SNDFILE_INCLUDE_DIR})
			target_link_libraries(DiskIO_UGens_supernova ${SNDFILE_LIBRARIES} ${MATH_LIBRARY} ${PTHREADS_LIBRARY})
			list(APPEND supernova_plugins DiskIO_UGens_supernova)
		else()
			message(SEND_ERROR "Cannot find libsndfile")
		endif(SNDFILE_FOUND)
	endif()
endif(SUPERNOVA)

foreach(ugen ${fft_ugens})
	target_link_libraries(${ugen} ${MATH_LIBRARY})

	if (APPLE)
		target_link_libraries(${ugen} "-framework vecLib")
	endif()
endforeach()

foreach (ugen ${uiugens})
    target_link_libraries(${ugen} ${PTHREADS_LIBRARY})
endforeach()

foreach (plugin ${plugins})
    if (WIN32)
        target_link_libraries(${plugin} wsock32 ws2_32)
    endif()
endforeach()


if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	find_package(X11)

	if(X11_FOUND)
		include_directories(X11_INCLUDE_DIR)

		foreach (ugen ${uiugens})
			target_link_libraries(${ugen} ${X11_X11_LIB})
		endforeach()
	elseif()
		message(SEND_ERROR "Cannot find libx11")
	endif()
elseif (APPLE)
	foreach (ugen ${uiugens})
		target_link_libraries(${ugen} "-framework CoreServices -framework Carbon")
	endforeach()

	if(NOT NO_LIBSNDFILE)
		target_link_libraries(DiskIO_UGens "-framework CoreServices")
	endif()

	add_definitions("-DSC_DARWIN")
endif()

if (APPLE)
	# determines the app name and app install location (scappbundlename, scappdir):
	include (${CMAKE_SOURCE_DIR}/cmake_modules/MacAppFolder.cmake)

	install(TARGETS ${plugins}
			DESTINATION "${scappauxresourcesdir}/plugins"
			PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE OWNER_WRITE)
else()
	install(TARGETS ${plugins}
			DESTINATION "lib/SuperCollider/plugins"
			PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

	if (SUPERNOVA)
		install(TARGETS ${supernova_plugins}
			DESTINATION "lib/supernova/plugins"
			PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	endif()
endif()
