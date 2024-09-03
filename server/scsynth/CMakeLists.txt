find_package(Sndfile)

# here we choose who provides us with the FFT lib
# this is done independently for scsynth and supernova
# on macOS scsynth uses vDSP implementation for FFT
# which is not thread-safe

if (APPLE)
    add_definitions("-DSC_FFT_VDSP")
    message(STATUS "FFT library (scsynth): vDSP")
else()
    find_package(FFTW3f 3.3)

    if (NOT FFTW3F_FOUND OR FFT_GREEN)
        message(STATUS "FFT library (scsynth): Green")
        set(FFT_GREEN 1)
        add_definitions("-DSC_FFT_GREEN")
    else()
        message(STATUS "Found fftw3f: ${FFTW3F_LIBRARY}")
        message(STATUS "FFT library (scsynth): FFTW")
        add_definitions("-DSC_FFT_FFTW")
    endif()
endif()

# Here we work out which audio API to use, from system type and/or user option.
if(AUDIOAPI STREQUAL "default")
	if(APPLE)
		set(AUDIOAPI coreaudio)
	elseif(WIN32)
		set(AUDIOAPI portaudio)
	else()
		set(AUDIOAPI jack)
	endif(APPLE)
endif()

if(NOT AUDIOAPI MATCHES "^(jack|coreaudio|portaudio|bela)$")
	message(FATAL_ERROR "Unrecognised audio API: ${AUDIOAPI}")
endif()

if(AUDIOAPI STREQUAL jack)
  # NOTE: this is a workaround for Windows if pkgconfig is not available
  if(WIN32 AND _JACK_LINK_LIBRARIES AND _JACK_INCLUDE_DIRS)
    set(JACK_LINK_LIBRARIES ${_JACK_LINK_LIBRARIES})
    set(JACK_INCLUDE_DIRS ${_JACK_INCLUDE_DIRS})
  else()
    include(FindPkgConfig)
    pkg_check_modules(JACK REQUIRED jack)
  endif()
  message(STATUS "Using JACK_LINK_LIBRARIES: ${JACK_LINK_LIBRARIES}")
  message(STATUS "Using JACK_INCLUDE_DIRS: ${JACK_INCLUDE_DIRS}")
elseif(AUDIOAPI STREQUAL portaudio AND SYSTEM_PORTAUDIO)
    find_package(Portaudio)
    if(NOT PORTAUDIO_FOUND)
      message(FATAL_ERROR "Portaudio selected as audio API, but development files not found")
    endif()
elseif(AUDIOAPI STREQUAL bela)
    # The Bela lib will have its own set of libraries and includes
    # However, the Bela audio backend for SC has some direct calls to Xenomai
    # Therefore we need to get flags for those as well
    find_package(Xenomai REQUIRED)
    find_package(Bela REQUIRED)
    message(STATUS "Found Bela libraries: ${BELA_LIBRARIES}")
    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
        # recommended compile flags for beaglebone etc; set here because bela api flag directly implies the architecture
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${BELA_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BELA_CXX_FLAGS}")
    endif()
endif()
message(STATUS "Audio API (scsynth): ${AUDIOAPI}")

set(scsynth_sources
	SC_BufGen.cpp

	SC_ComPort.cpp
	SC_CoreAudio.cpp
	SC_Graph.cpp
	SC_GraphDef.cpp
	SC_Group.cpp
	SC_HiddenWorld.h
	SC_Lib_Cintf.cpp
	SC_Lib.cpp
	SC_MiscCmds.cpp
	SC_Node.cpp
	SC_Rate.cpp
	SC_SequencedCommand.cpp
	SC_Str4.cpp
	SC_Unit.cpp
	SC_UnitDef.cpp
	SC_World.cpp
	Rendezvous.cpp

	${CMAKE_SOURCE_DIR}/common/SC_Filesystem_macos.cpp
	${CMAKE_SOURCE_DIR}/common/SC_Filesystem_win.cpp
	${CMAKE_SOURCE_DIR}/common/SC_Filesystem_unix.cpp
	${CMAKE_SOURCE_DIR}/common/SC_Filesystem_iphone.cpp

	${CMAKE_SOURCE_DIR}/common/SC_fftlib.cpp
	${CMAKE_SOURCE_DIR}/common/SC_AllocPool.cpp
	${CMAKE_SOURCE_DIR}/common/SC_Errors.cpp
	${CMAKE_SOURCE_DIR}/common/SC_Reply.cpp
	${CMAKE_SOURCE_DIR}/common/SC_StringBuffer.cpp
	${CMAKE_SOURCE_DIR}/common/SC_StringParser.cpp
	${CMAKE_SOURCE_DIR}/common/Samp.cpp
	${CMAKE_SOURCE_DIR}/common/sc_popen.cpp
	${CMAKE_SOURCE_DIR}/common/SC_ServerBootDelayWarning.cpp
)

if(APPLE)
	set_property(SOURCE ${CMAKE_SOURCE_DIR}/common/SC_Filesystem_macos.cpp PROPERTY COMPILE_FLAGS -xobjective-c++)

	list(APPEND scsynth_sources
		${CMAKE_SOURCE_DIR}/common/SC_Apple.mm
	)
	set_source_files_properties( ${CMAKE_SOURCE_DIR}/common/SC_Apple.mm
	    PROPERTIES COMPILE_FLAGS "-x objective-c++ -fobjc-exceptions"
	)
endif()

if(WIN32)
	list(APPEND scsynth_sources ${CMAKE_SOURCE_DIR}/common/SC_Win32Utils.cpp)
endif()

if (FFT_GREEN)
	list(APPEND scsynth_sources ../../common/fftlib.c)
endif()

if(AUDIOAPI STREQUAL bela)
    list(APPEND scsynth_sources ../../common/XenomaiLock.cpp)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include/common
                    ${CMAKE_SOURCE_DIR}/common
                    ${CMAKE_SOURCE_DIR}/include/server
                    ${CMAKE_SOURCE_DIR}/include/plugin_interface
                    .
)

include_directories (${CMAKE_SOURCE_DIR}/external_libraries/boost_sync/include)

if (AUDIOAPI STREQUAL jack)
	list(APPEND scsynth_sources SC_Jack.cpp)
	add_definitions("-DSC_AUDIO_API=SC_AUDIO_API_JACK")
	include_directories(${JACK_INCLUDE_DIRS})
elseif (AUDIOAPI STREQUAL portaudio)
	list(APPEND scsynth_sources SC_PortAudio.cpp ${CMAKE_SOURCE_DIR}/common/SC_PaUtils.cpp)
	add_definitions("-DSC_AUDIO_API=SC_AUDIO_API_PORTAUDIO" ${PORTAUDIO_DEFINITIONS})
	include_directories(${PORTAUDIO_INCLUDE_DIRS})
elseif (AUDIOAPI STREQUAL bela)
    list(APPEND scsynth_sources SC_Bela.cpp)
    add_definitions("-DSC_AUDIO_API=SC_AUDIO_API_BELA" "-DSC_BELA" ${XENOMAI_DEFINITIONS} ${BELA_DEFINITIONS})
    include_directories(${XENOMAI_INCLUDE_DIRS} ${BELA_INCLUDE_DIRS})
endif()

set (FINAL_BUILD 0) # disable final build for scsynth

if (LIBSCSYNTH)
	set (LIBSCSYNTH_TYPE SHARED)
else()
	set (LIBSCSYNTH_TYPE STATIC)
endif()

file(GLOB_RECURSE all_headers ./*.h*)

if (FINAL_BUILD)
	CREATE_FINAL_FILE(libscsynth_final.cpp ${scsynth_sources} ${all_headers})
	add_library(libscsynth ${LIBSCSYNTH_TYPE} libscsynth_final.cpp)
else()
	add_library(libscsynth ${LIBSCSYNTH_TYPE} ${scsynth_sources} ${all_headers})
endif()


if(LIBSCSYNTH)
        target_compile_definitions(libscsynth PRIVATE   BUILDING_SCSYNTH)
        target_compile_definitions(libscsynth INTERFACE BUILDING_SCSYNTH)
endif()

target_compile_definitions(libscsynth PUBLIC  SC_MEMORY_ALIGNMENT=32)

target_link_libraries(libscsynth tlsf)

find_library(DL NAMES dl)
if(DL)
    target_link_libraries(libscsynth ${DL})
endif()

if(NOVA_SIMD)
	target_compile_definitions(libscsynth PUBLIC NOVA_SIMD)
endif()

if(SNDFILE_FOUND)
	if(SNDFILE_HAS_VORBIS)
		target_compile_definitions(libscsynth PUBLIC SNDFILE_HAS_VORBIS)
	endif()
	if(SNDFILE_HAS_OPUS)
		target_compile_definitions(libscsynth PUBLIC SNDFILE_HAS_OPUS)
	endif()
	if(SNDFILE_HAS_MPEG)
		target_compile_definitions(libscsynth PUBLIC SNDFILE_HAS_MPEG)
	endif()
	target_link_libraries(libscsynth ${SNDFILE_LIBRARIES})
	target_include_directories(libscsynth PUBLIC ${SNDFILE_INCLUDE_DIR})
elseif(NOT NO_LIBSNDFILE)
	message(SEND_ERROR "Cannot find libsndfile")
endif(SNDFILE_FOUND)

if(UNIX AND NOT APPLE)
	target_compile_definitions(libscsynth PUBLIC "SC_PLUGIN_DIR=\"${LINUX_SC_PLUGIN_DIR}\"")
endif()

if(NOT NO_AVAHI)
	if(APPLE)
		target_compile_definitions(libscsynth PUBLIC USE_RENDEZVOUS=1)
	else()
		find_package(Avahi)
		if(AVAHI_FOUND)
			target_compile_definitions(libscsynth PUBLIC USE_RENDEZVOUS=1 HAVE_AVAHI=1)
			target_link_libraries(libscsynth ${AVAHI_LIBRARIES})
			target_include_directories(libscsynth PUBLIC ${AVAHI_INCLUDE_DIRS})
		endif()
	endif()
endif()


if (AUDIOAPI STREQUAL jack)
	target_link_libraries(libscsynth ${JACK_LINK_LIBRARIES})
elseif(AUDIOAPI STREQUAL portaudio)
	target_link_libraries(libscsynth ${PORTAUDIO_LIBRARIES})
elseif(AUDIOAPI STREQUAL bela)
    target_link_libraries(libscsynth ${XENOMAI_LIBRARIES} ${BELA_LIBRARIES} belaextra)
elseif(AUDIOAPI STREQUAL coreaudio)
	target_link_libraries(libscsynth "-framework CoreAudio")
endif()

target_link_libraries(libscsynth boost_system_lib boost_filesystem_lib)
target_include_directories(libscsynth PUBLIC ${boost_include_dirs})

if (WIN32)
	target_link_libraries(libscsynth wsock32 ws2_32 winmm)
	target_compile_definitions(libscsynth PUBLIC UNICODE _UNICODE)
endif()

if(NOT WIN32)
set_property(TARGET libscsynth PROPERTY OUTPUT_NAME scsynth)
endif()

if (LIBSCSYNTH)
	# These two properties are ABI version info, not sc version:
	set_property(TARGET libscsynth PROPERTY VERSION     1.0.0)
	set_property(TARGET libscsynth PROPERTY SOVERSION   1)
endif()

if (FFTW3F_FOUND)
	target_include_directories(libscsynth PUBLIC ${FFTW3F_INCLUDE_DIR})
	target_link_libraries(libscsynth ${FFTW3F_LIBRARY})
endif()

if (APPLE)
	target_link_libraries(libscsynth "-framework Accelerate -framework CoreServices"
                                        "-framework Foundation -framework AppKit")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(libscsynth rt)
endif()

add_executable(scsynth
    scsynth_main.cpp

    # these files contain code only used in main()
    ${CMAKE_SOURCE_DIR}/common/SC_ServerBootDelayWarning.cpp
    $<$<BOOL:${APPLE}>: ${CMAKE_SOURCE_DIR}/common/SC_AppleEventLoop.mm >
    )
target_link_libraries(scsynth libscsynth)

if(AUDIOAPI STREQUAL bela)
    target_link_libraries(scsynth ${XENOMAI_LIBRARIES} ${BELA_LIBRARIES} belaextra)
endif()

if (PTHREADS_FOUND)
    target_link_libraries(scsynth ${PTHREADS_LIBRARIES})
endif()

if(LTO)
    set_property(TARGET scsynth libscsynth
                 APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

    set_property(TARGET scsynth
                 APPEND PROPERTY LINK_FLAGS "-flto -flto-report -fwhole-program")

    set_property(TARGET libscsynth
                 APPEND PROPERTY LINK_FLAGS "-flto -flto-report")
endif()

if (LIBSCSYNTH)
	set(INSTALL_TARGETS scsynth libscsynth)
else()
	set(INSTALL_TARGETS scsynth)
endif()

if(APPLE)
    if(scappauxresourcesdir)
        add_custom_command(TARGET scsynth POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/${scappauxresourcesdir}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:scsynth> ${CMAKE_INSTALL_PREFIX}/${scappauxresourcesdir}
            )
    endif()
elseif(WIN32)
    if(NOT MSVC)
      target_link_libraries(scsynth "-municode")
      set_target_properties(scsynth libscsynth PROPERTIES RUNTIME_OUTPUT_DIRECTORY "$<CONFIG>")
    endif(NOT MSVC)

    if(FFTW3F_FOUND)
      file(GLOB FFTW3F_DLL "${FFTW3F_LIBRARY_DIR}/*fftw3f*.dll")
      add_custom_command(TARGET scsynth
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FFTW3F_DLL}" $<TARGET_FILE_DIR:scsynth>
      )
    endif(FFTW3F_FOUND)

    if(SNDFILE_FOUND)
      file(GLOB SNDFILE_DLL "${SNDFILE_LIBRARY_DIR}/*sndfile*.dll")
      add_custom_command(TARGET scsynth
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SNDFILE_DLL}" $<TARGET_FILE_DIR:scsynth>
      )
    endif(SNDFILE_FOUND)

    add_custom_command(TARGET scsynth
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:scsynth> $<TARGET_FILE_DIR:sclang>
      COMMENT "Adding scynth to target sclang"
    )

    if(SC_IDE)
      add_custom_command(TARGET scsynth
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:scsynth> $<TARGET_FILE_DIR:SuperCollider>
        COMMENT "Adding scynth to target SuperCollider"
      )
    endif(SC_IDE)

    install(TARGETS ${INSTALL_TARGETS}
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        DESTINATION "${SC_WIN_BUNDLE_NAME}"
    )

else()
	install(TARGETS ${INSTALL_TARGETS}
			RUNTIME DESTINATION "bin"
			LIBRARY DESTINATION "lib${LIB_SUFFIX}"
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if(${JACK_USE_METADATA_API})
	target_compile_definitions(libscsynth PUBLIC "-DSC_JACK_USE_METADATA_API")
endif()
