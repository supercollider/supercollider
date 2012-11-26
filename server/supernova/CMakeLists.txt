if (FFT_GREEN)
    add_definitions("-DSC_FFT_GREEN")
elseif (APPLE)
    add_definitions("-DSC_FFT_VDSP")
else()
    find_package(FFTW3f 3.3)
    if (FFTW3F_FOUND)
        add_definitions("-DSC_FFT_FFTW")
        include_directories (${FFTW3F_INCLUDE_DIR})
    else()
        message(SEND_ERROR "Cannot find libfftw3f")
    endif()
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-4096")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fomit-frame-pointer")

  if(APPLE)
    if (CMAKE_SIZEOF_VOID_P MATCHES "4")
      # cmpxchg8b is available on all intel apples
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i686 -msse -msse2 -mfpmath=sse")
    endif()
  endif()
endif(CMAKE_COMPILER_IS_GNUCXX)


if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_INTEL OR CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(FATAL_ERROR "unknown compiler. supernova cannot be compiled")
endif()

if(APPLE AND CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()


if(SN_MEMORY_DEBUGGING)
    set_property(DIRECTORY .
                 APPEND
                 PROPERTY COMPILE_DEFINITIONS_DEBUG NOVA_MEMORY_DEBUGGING)
endif()

if(NOVA_SIMD)
    add_definitions(-DNOVA_SIMD)
endif()


include_directories(${CMAKE_SOURCE_DIR}/external_libraries/boost_endian
                    ${CMAKE_SOURCE_DIR}/external_libraries/oscpack
                    ${CMAKE_SOURCE_DIR}/external_libraries/TLSF-2.4.6/src
                    )

if(APPLE)
    include_directories(${CMAKE_SOURCE_DIR}/external_libraries/libsndfile)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include/plugin_interface
                    ${CMAKE_SOURCE_DIR}/include/server
                    ${CMAKE_SOURCE_DIR}/include/common
                    .
                    )

set(libsupernova_src
    sc/sc_synth_definition.cpp
    sc/sc_osc_handler.cpp
    sc/sc_plugin_interface.cpp
    sc/sc_synth.cpp
    sc/sc_synthdef.cpp
    sc/sc_ugen_factory.cpp
    ../scsynth/Samp.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_fftlib.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_DirUtils.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_StringParser.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_StandAloneInfo_Darwin.cpp
    server/buffer_manager.cpp
    server/dsp_context.cpp
    server/memory_pool.cpp
    server/node_graph.cpp
    server/server.cpp
    server/server_args.cpp
)

if(WIN32)
    list(APPEND libsupernova_src ${CMAKE_SOURCE_DIR}/common/SC_Win32Utils.cpp)
endif()


if(FFT_GREEN)
    list(APPEND libsupernova_src ../../common/fftlib.c)
endif()

file(GLOB_RECURSE supernova_headers *hpp)

if(COMPACT_BUILD)
  CREATE_FINAL_FILE(libsupernova_final.cpp ${libsupernova_src})
  add_library (libsupernova STATIC libsupernova_final.cpp)
else()
  add_library (libsupernova STATIC ${libsupernova_src})
endif()

set_property(TARGET libsupernova
             PROPERTY OUTPUT_NAME supernova)

add_executable(supernova server/main.cpp ${supernova_headers})

set_property(TARGET libsupernova supernova
             APPEND
             PROPERTY COMPILE_DEFINITIONS SUPERNOVA)

find_library(DL NAMES dl)
if (DL)
    set_property(TARGET libsupernova
                 APPEND
                 PROPERTY COMPILE_DEFINITIONS DLOPEN)
    target_link_libraries(libsupernova ${DL})
endif()

if (WIN32 OR APPLE)
    find_package(Portaudio)
    if(NOT PORTAUDIO_FOUND)
        message(FATAL_ERROR "Portaudio missing")
    endif()
    add_definitions(-DPORTAUDIO_BACKEND ${PORTAUDIO_DEFINITIONS})
    include_directories(${PORTAUDIO_INCLUDE_DIRS})
    target_link_libraries(libsupernova ${PORTAUDIO_LIBRARIES})
else()
    find_library(JACK NAMES jack)
    if (JACK)
        set_property(TARGET libsupernova supernova
                    APPEND
                    PROPERTY COMPILE_DEFINITIONS JACK_BACKEND)
        target_link_libraries(libsupernova ${JACK})
        include_directories(${JACK_INCLUDE_DIRS})
    endif()
endif()

find_package(Sndfile)
if (NOT SNDFILE_FOUND)
    message(SEND_ERROR "Cannot find libsndfile")
else()
    target_link_libraries(libsupernova ${SNDFILE_LIBRARIES})
    include_directories(${SNDFILE_INCLUDE_DIR})
endif()

if (FFTW3F_FOUND)
	target_link_libraries(libsupernova ${FFTW3F_LIBRARY})
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(libsupernova rt)
endif()

target_link_libraries(libsupernova oscpack tlsf ${PTHREADS_LIBRARIES})

if (Boost_FOUND)
    target_link_libraries(libsupernova   ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY})
else()
    target_link_libraries(libsupernova boost_system boost_filesystem boost_program_options)
endif()

target_link_libraries(supernova libsupernova)

if(WIN32)
    target_link_libraries(libsupernova wsock32 ws2_32 winmm)
endif()

if(APPLE)
	# determines the app name and app install location (scappbundlename, scappdir):
	include (${CMAKE_SOURCE_DIR}/cmake_modules/MacAppFolder.cmake)

	install(TARGETS supernova
			DESTINATION ${scappauxresourcesdir}
			PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE OWNER_WRITE)
else()

install(TARGETS supernova
        DESTINATION "bin"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if(LTO)
    set_property(TARGET supernova libsupernova
                 APPEND PROPERTY COMPILE_FLAGS "-flto -flto-report")

    set_property(TARGET supernova libsupernova
                 APPEND PROPERTY LINK_FLAGS "-flto -flto-report -fwhole-program")
endif()

if (APPLE)
    target_link_libraries(libsupernova "-framework vecLib")
    target_link_libraries(libsupernova "-framework CoreAudio -framework CoreServices")
endif()
