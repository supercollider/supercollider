if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-4096")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fomit-frame-pointer")

  if(APPLE)
    if(CMAKE_SIZEOF_VOID_P MATCHES "4")
      # cmpxchg8b is available on all intel apples
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i686 -msse -msse2 -mfpmath=sse")
    endif()
  endif()

endif(CMAKE_COMPILER_IS_GNUCXX)

# here we choose who provides us with the FFT lib
# this is done independently for scsynth and supernova
# scsynth uses vDSP implementation for FFT on macOS
# however current implementation of vDSP is not thread-safe
# so we use FFTW with supernova on macOS
# once vDSP implementation becomes thread-safe
# this logic can be reworked to use vDSP on macOS

find_package(FFTW3f 3.3)

if (NOT FFTW3F_FOUND OR FFT_GREEN)
    message(STATUS "FFT library (supernova): Green")
    set(FFT_GREEN 1)
    add_definitions("-DSC_FFT_GREEN")
else()
    message(STATUS "Found fftw3f: ${FFTW3F_LIBRARY}")
    message(STATUS "FFT library (supernova): FFTW")
    add_definitions("-DSC_FFT_FFTW")
endif()

set(libsupernova_src
    sc/sc_synth_definition.cpp
    sc/sc_osc_handler.cpp
    sc/sc_plugin_interface.cpp
    sc/sc_synth.cpp
    sc/sc_synthdef.cpp
    sc/sc_ugen_factory.cpp

    ${CMAKE_SOURCE_DIR}/common/SC_Filesystem_macos.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_Filesystem_win.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_Filesystem_unix.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_Filesystem_iphone.cpp

    ${CMAKE_SOURCE_DIR}/common/Samp.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_fftlib.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_StringParser.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_ServerBootDelayWarning.cpp
    server/buffer_manager.cpp
    server/memory_pool.cpp
    server/node_graph.cpp
    server/server.cpp
    server/server_args.cpp
)

if(APPLE)
  list( APPEND libsupernova_src ${CMAKE_SOURCE_DIR}/common/SC_Apple.mm )

  set_property(SOURCE ${CMAKE_SOURCE_DIR}/common/SC_Filesystem_macos.cpp PROPERTY COMPILE_FLAGS -xobjective-c++)
endif()


if (FFT_GREEN)
    list(APPEND libsupernova_src ../../common/fftlib.c)
endif()

if(WIN32)
    list(APPEND libsupernova_src ${CMAKE_SOURCE_DIR}/common/SC_Win32Utils.cpp)
endif()

file(GLOB_RECURSE supernova_headers *hpp)

if(COMPACT_BUILD)
  CREATE_FINAL_FILE(libsupernova_final.cpp ${libsupernova_src})
  add_library (libsupernova STATIC libsupernova_final.cpp)
else()
  add_library (libsupernova STATIC ${libsupernova_src})
endif()

if(LINUX AND CMAKE_COMPILER_IS_CLANG)
    message(STATUS "Clang on Linux detected, linking to libatomic")
    target_link_libraries(libsupernova atomic)
endif()
if(NOT WIN32)
  set_property(TARGET libsupernova
               PROPERTY OUTPUT_NAME supernova)
endif()

target_link_libraries(libsupernova oscpack tlsf ${PTHREADS_LIBRARIES} boost_thread_lib)

target_compile_definitions(libsupernova PUBLIC SUPERNOVA)
target_include_directories(libsupernova PUBLIC
                    ${CMAKE_SOURCE_DIR}/include/plugin_interface
                    ${CMAKE_SOURCE_DIR}/include/common
                    ${CMAKE_SOURCE_DIR}/common
                    ${CMAKE_SOURCE_DIR}/include/server
                    ${CMAKE_SOURCE_DIR}/server/scsynth
                    ${CMAKE_SOURCE_DIR}/external_libraries/boost_endian
                    ${CMAKE_SOURCE_DIR}/external_libraries/boost_sync/include
                    ${CMAKE_SOURCE_DIR}/external_libraries/nova-tt
                    .
                    )

if(SN_MEMORY_DEBUGGING)
    target_compile_definitions(libsupernova PUBLIC NOVA_MEMORY_DEBUGGING)
endif()

if(NOVA_SIMD)
    target_compile_definitions(libsupernova PUBLIC NOVA_SIMD)
    target_include_directories(libsupernova PUBLIC ${CMAKE_SOURCE_DIR}/external_libraries)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD|DragonFly|OpenBSD|NetBSD")
    target_compile_definitions(libsupernova PUBLIC DLOPEN)
else()
  find_library(DL NAMES dl)
  if(DL)
      target_compile_definitions(libsupernova PUBLIC DLOPEN)
      target_link_libraries(libsupernova ${DL})
  endif()
endif()

if(AUDIOAPI STREQUAL "default")
    if(APPLE)
        set(AUDIOAPI portaudio)
    elseif(WIN32)
        set(AUDIOAPI portaudio)
    else()
        set(AUDIOAPI jack)
    endif(APPLE)
elseif(AUDIOAPI STREQUAL bela)
    message(FATAL_ERROR "supernova is not supported on Bela")
endif()

if(NOT AUDIOAPI MATCHES "^(jack|coreaudio|portaudio)$")
    message(FATAL_ERROR "Unrecognized audio API: ${AUDIOAPI}")
endif()

if(AUDIOAPI STREQUAL "coreaudio")
    set(AUDIOAPI portaudio)
    message(FATAL_ERROR "supernova: coreaudio API not implemented yet, use 'default' or 'portaudio' instead")
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
    target_compile_definitions(libsupernova PUBLIC JACK_BACKEND)
    target_link_libraries(libsupernova ${JACK_LINK_LIBRARIES})
    target_include_directories(libsupernova PUBLIC ${JACK_INCLUDE_DIRS})
elseif(AUDIOAPI STREQUAL portaudio AND SYSTEM_PORTAUDIO)
    find_package(Portaudio REQUIRED)
endif()
message(STATUS "Audio API (supernova): ${AUDIOAPI}")

if(AUDIOAPI STREQUAL portaudio)
    target_sources(libsupernova PUBLIC ${CMAKE_SOURCE_DIR}/common/SC_PaUtils.cpp)
    target_include_directories(libsupernova PUBLIC ${PORTAUDIO_INCLUDE_DIRS})
    target_compile_definitions(libsupernova PUBLIC PORTAUDIO_BACKEND ${PORTAUDIO_DEFINITIONS})
    target_link_libraries(libsupernova ${PORTAUDIO_LIBRARIES})
endif()

find_package(Sndfile)
if (NOT SNDFILE_FOUND)
    message(SEND_ERROR "Cannot find libsndfile")
else()
    if(SNDFILE_HAS_VORBIS)
        target_compile_definitions(libsupernova PUBLIC SNDFILE_HAS_VORBIS)
    endif()
    if(SNDFILE_HAS_OPUS)
        target_compile_definitions(libsupernova PUBLIC SNDFILE_HAS_OPUS)
    endif()
    if(SNDFILE_HAS_MPEG)
        target_compile_definitions(libsupernova PUBLIC SNDFILE_HAS_MPEG)
    endif()
    target_link_libraries(libsupernova ${SNDFILE_LIBRARIES})
    target_include_directories(libsupernova PUBLIC ${SNDFILE_INCLUDE_DIR})
endif()

if (FFTW3F_FOUND)
    target_include_directories(libsupernova PUBLIC ${FFTW3F_INCLUDE_DIR})
    target_link_libraries(libsupernova ${FFTW3F_LIBRARY})
    set_property(TARGET libsupernova PROPERTY SUPERNOVA_FFTW ON) # for fixing linking in macdeployqt
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(libsupernova rt)
endif()

target_link_libraries(libsupernova boost_system_lib boost_filesystem_lib boost_program_options_lib)
target_include_directories(libsupernova PUBLIC ${boost_include_dirs})

if(CMAKE_COMPILER_IS_GNUCXX)
    if( ${_gcc_version} VERSION_GREATER 5 )
      target_link_libraries( libsupernova atomic )
    endif()
endif()

if(WIN32)
    target_link_libraries(libsupernova wsock32 ws2_32 winmm)
    target_compile_definitions(libsupernova PUBLIC UNICODE _UNICODE)
endif()


add_executable(supernova
    server/main.cpp
    ${supernova_headers}

    # these files contain code only used in main()
    ${CMAKE_SOURCE_DIR}/common/SC_ServerBootDelayWarning.cpp
    $<$<BOOL:${APPLE}>: ${CMAKE_SOURCE_DIR}/common/SC_AppleEventLoop.mm >
    )
target_link_libraries(supernova libsupernova)

if(WIN32)
    if(NOT MSVC)
      target_link_libraries(supernova "-municode")
      set_target_properties(supernova libsupernova PROPERTIES RUNTIME_OUTPUT_DIRECTORY "$<CONFIG>")
    endif(NOT MSVC)

    if(FFTW3F_FOUND)
      file(GLOB FFTW3F_DLL "${FFTW3F_LIBRARY_DIR}/*fftw3f*.dll")
      add_custom_command(TARGET supernova
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FFTW3F_DLL}" $<TARGET_FILE_DIR:supernova>
      )
    endif(FFTW3F_FOUND)

    if(SNDFILE_FOUND)
      file(GLOB SNDFILE_DLL "${SNDFILE_LIBRARY_DIR}/*sndfile*.dll")
      add_custom_command(TARGET supernova
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SNDFILE_DLL}" $<TARGET_FILE_DIR:supernova>
      )
    endif(SNDFILE_FOUND)

    add_custom_command(TARGET supernova
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:supernova> $<TARGET_FILE_DIR:scsynth>
      COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:supernova> $<TARGET_FILE_DIR:sclang>
      COMMENT "Copying supernova to targets scsynth and sclang"
    )

    if(SC_IDE)
      add_custom_command(TARGET supernova
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:supernova> $<TARGET_FILE_DIR:SuperCollider>
        COMMENT "Copying supernova to target SuperCollider"
      )
    endif(SC_IDE)

    install(TARGETS supernova
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        DESTINATION ${SC_WIN_BUNDLE_NAME}
    )

elseif(APPLE)
    if(scappauxresourcesdir)
        add_custom_command(TARGET supernova POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/${scappauxresourcesdir}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:supernova> ${CMAKE_INSTALL_PREFIX}/${scappauxresourcesdir}
            )
    endif()
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
    target_link_libraries(libsupernova "-framework Accelerate -framework CoreAudio -framework CoreServices"
                                        "-framework Foundation -framework ApplicationServices -framework AppKit")
endif()

if(${JACK_USE_METADATA_API})
    target_compile_definitions(libsupernova PUBLIC "-DSC_JACK_USE_METADATA_API")
endif()



if(${LIB_SUFFIX})
    add_definitions(-DLIB_SUFFIX="${LIB_SUFFIX}")
else()
    add_definitions(-DLIB_SUFFIX="")
endif()

if(UNIX AND NOT APPLE)
    target_compile_definitions(libsupernova PUBLIC "SC_PLUGIN_DIR=\"${LINUX_SC_PLUGIN_DIR}\"")
endif()
