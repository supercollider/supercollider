cmake_minimum_required( VERSION 3.1 )

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

if(APPLE)
    include_directories(${CMAKE_SOURCE_DIR}/external_libraries/libsndfile)
endif()

set(libsupernova_src
    sc/sc_synth_definition.cpp
    sc/sc_osc_handler.cpp
    sc/sc_plugin_interface.cpp
    sc/sc_synth.cpp
    sc/sc_synthdef.cpp
    sc/sc_ugen_factory.cpp
    ${CMAKE_SOURCE_DIR}/common/Samp.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_fftlib.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_DirUtils.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_StringParser.cpp
    ${CMAKE_SOURCE_DIR}/common/SC_StandAloneInfo_Darwin.cpp
    server/buffer_manager.cpp
    server/memory_pool.cpp
    server/node_graph.cpp
    server/server.cpp
    server/server_args.cpp
)

if(APPLE)
  list( APPEND libsupernova_src ${CMAKE_SOURCE_DIR}/common/SC_Apple.mm )

  set_property(SOURCE ${CMAKE_SOURCE_DIR}/common/SC_DirUtils.cpp PROPERTY COMPILE_FLAGS -xobjective-c++)
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

set_property(TARGET libsupernova PROPERTY CXX_STANDARD 14)


if(NOT WIN32)
  set_property(TARGET libsupernova
               PROPERTY OUTPUT_NAME supernova)
endif()

target_link_libraries(libsupernova oscpack tlsf ${PTHREADS_LIBRARIES} boost_thread)

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

find_library(DL NAMES dl)
if(DL)
    target_compile_definitions(libsupernova PUBLIC DLOPEN)
    target_link_libraries(libsupernova ${DL})
endif()

if(APPLE)
    find_package(Portaudio)
    if(NOT PORTAUDIO_FOUND)
        message(FATAL_ERROR "Portaudio missing")
    endif()
    target_compile_definitions(libsupernova PUBLIC PORTAUDIO_BACKEND ${PORTAUDIO_DEFINITIONS})
    target_include_directories(libsupernova PUBLIC ${PORTAUDIO_INCLUDE_DIRS})
    target_link_libraries(libsupernova ${PORTAUDIO_LIBRARIES})
elseif (WIN32)
    target_compile_definitions(libsupernova PUBLIC PORTAUDIO_BACKEND -DPORTAUDIO_BACKEND)
    target_include_directories(libsupernova PUBLIC ${PORTAUDIO_INCLUDE_DIRS})
    if(MSYS)
        target_link_libraries(libsupernova ${PORTAUDIO_LIBRARIES})
    else()
        target_link_libraries(libsupernova portaudio)
    endif()
else()
    find_library(JACK NAMES jack)
    if (JACK)
        target_compile_definitions(libsupernova PUBLIC JACK_BACKEND)
        target_link_libraries(libsupernova ${JACK})
        target_include_directories(libsupernova PUBLIC ${JACK_INCLUDE_DIRS})
    endif()
endif()

find_package(Sndfile)
if (NOT SNDFILE_FOUND)
    message(SEND_ERROR "Cannot find libsndfile")
else()
    target_link_libraries(libsupernova ${SNDFILE_LIBRARIES})
    target_include_directories(libsupernova PUBLIC ${SNDFILE_INCLUDE_DIR})
endif()

if (FFTW3F_FOUND)
    target_include_directories(libsupernova PUBLIC ${FFTW3F_INCLUDE_DIR})
    target_link_libraries(libsupernova ${FFTW3F_LIBRARY})
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(libsupernova rt)
endif()


if(Boost_FOUND)
    target_link_libraries(libsupernova ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY})
    target_include_directories(libsupernova PUBLIC ${Boost_INCLUDE_DIRS})
else()
    target_include_directories(libsupernova PUBLIC ${CMAKE_SOURCE_DIR}/external_libraries/boost)
    target_link_libraries(libsupernova boost_system boost_filesystem boost_program_options)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    if( ${_gcc_version} VERSION_GREATER 5 )
      target_link_libraries( libsupernova atomic )
    endif()
endif()

if(WIN32)
    target_link_libraries(libsupernova wsock32 ws2_32 winmm)
endif()


add_executable(supernova server/main.cpp ${supernova_headers})
target_link_libraries(supernova libsupernova)

if(WIN32)
    if(NOT MSVC)
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
    add_custom_command(TARGET supernova POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:SuperCollider>/../Resources/
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:supernova> $<TARGET_FILE_DIR:SuperCollider>/../Resources)
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
    target_link_libraries(libsupernova "-framework Accelerate")
    target_link_libraries(libsupernova "-framework CoreAudio -framework CoreServices -framework Foundation -framework ApplicationServices")
endif()

if(${JACK_USE_METADATA_API})
    target_compile_definitions(libsupernova PUBLIC "-DSC_JACK_USE_METADATA_API")
endif()
