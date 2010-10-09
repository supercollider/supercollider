# here we choose who provides us with the FFT lib
if (FFT_GREEN)
	add_definitions("-DSC_FFT_GREEN")
elseif (APPLE)
	add_definitions("-DSC_FFT_VDSP")
else()
	find_package(FFTW3f)

	if (FFTW3F_FOUND)
		add_definitions("-DSC_FFT_FFTW")
	else()
		message(SEND_ERROR "Cannot find libfftw3f")
	endif()
endif()

if(NOT NO_LIBSNDFILE)
	find_package(Sndfile)
endif()

include_directories(../../Headers/plugin_interface
					../../Headers/common
					../../Headers/server)

if (NOVA_SIMD)
	include_directories(../../include/nova-simd)
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

	KeyboardUGens.cpp
	MouseUGens.cpp
)

set(plugins "")

set(CMAKE_SHARED_MODULE_PREFIX "")
if(APPLE OR WIN32)
	set(CMAKE_SHARED_MODULE_SUFFIX ".scx")
endif()

if (${CMAKE_COMPILER_IS_GNUCXX})
	add_definitions("-fno-finite-math-only" "-fno-rtti" "-fno-exceptions")
endif()


foreach(plugin ${plugin_sources})
	string(REPLACE .cpp "" plugin_name ${plugin} )
	add_library(${plugin_name} MODULE ${plugin})
	list(APPEND plugins ${plugin_name})
endforeach(plugin)

set(scfft_sources   ../common/SC_fftlib.cpp   SCComplex.cpp)
if (FFT_GREEN)
	set(scfft_sources   ${scfft_sources} ../common/fftlib.c)
endif()

add_library(FFT_UGens MODULE
			FFTInterfaceTable.cpp
			FFT_UGens.cpp
			PV_UGens.cpp
			PartitionedConvolution.cpp
			${scfft_sources}
			)

add_library(PV_ThirdParty MODULE
			Convolution.cpp
			FFT2InterfaceTable.cpp
			FeatureDetection.cpp
			PV_ThirdParty.cpp
			${scfft_sources}
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
			${scfft_sources}
			)

if(NOT NO_LIBSNDFILE)
	add_library(DiskIO_UGens MODULE
			DiskIO_UGens.cpp
			../server/SC_SyncCondition.cpp)

	if(SNDFILE_FOUND)
		include_directories(${SNDFILE_INCLUDE_DIR})
		target_link_libraries(DiskIO_UGens ${SNDFILE_LIBRARIES} m pthread)
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

foreach(ugen ${fft_ugens})
	target_link_libraries(${ugen} m)

	if (FFTW3F_FOUND)
		target_link_libraries(${ugen} ${FFTW3F_LIBRARY})
	endif()

	if (APPLE)
		target_link_libraries(${ugen} "-framework vecLib")
	endif()

endforeach()


if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	find_package(X11)

	if(X11_FOUND)
		include_directories(X11_INCLUDE_DIR)

		target_link_libraries(KeyboardUGens ${X11_X11_LIB})
		target_link_libraries(MouseUGens ${X11_X11_LIB})
	elseif()
		message(SEND_ERROR "Cannot find libx11")
	endif()
elseif (APPLE)
	target_link_libraries(KeyboardUGens "-framework CoreServices -framework Carbon")
	target_link_libraries(MouseUGens "-framework CoreServices -framework Carbon")
	if(NOT NO_LIBSNDFILE)
		target_link_libraries(DiskIO_UGens "-framework CoreServices")
	endif()

	add_definitions("-DSC_DARWIN")
endif()

if (APPLE)
	# determines the app name and app install location (scappbundlename, scappdir):
	include (../../cmake_modules/MacAppFolder.cmake)

	install(TARGETS ${plugins}
			DESTINATION "${scappauxresourcesdir}/plugins"
			PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE OWNER_WRITE)
else()
	install(TARGETS ${plugins}
			DESTINATION "lib/SuperCollider/plugins"
			PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()
