add_subdirectory(sc)

set(libsupernova_src
    sc/sc_osc_handler.cpp
    sc/sc_plugin_interface.cpp
    sc/sc_synth.cpp
    sc/sc_synth_prototype.cpp
    sc/sc_synthdef.cpp
    sc/sc_ugen_factory.cpp
    sc/supercollider/common/Source/server/Samp.cpp
    server/buffer_manager.cpp
    server/dsp_context.cpp
    server/memory_pool.cpp
    server/node_graph.cpp
    server/server.cpp
    server/server_args.cpp
)

if(COMPACT_BUILD)
  CREATE_FINAL_FILE(libsupernova_final.cpp ${libsupernova_src})
  add_library (libsupernova STATIC libsupernova_final.cpp)
else()
  add_library (libsupernova STATIC ${libsupernova_src})
endif()

set_property(TARGET libsupernova
             PROPERTY OUTPUT_NAME supernova)

add_executable(supernova server/main.cpp)

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

find_library(JACK NAMES jack)
if (JACK)
    set_property(TARGET libsupernova supernova
                 APPEND
                 PROPERTY COMPILE_DEFINITIONS JACK_BACKEND)
    target_link_libraries(libsupernova ${JACK})
endif()

find_library(SNDFILE NAMES sndfile)
if (NOT SNDFILE)
    message(SEND_ERROR "Cannot find libsndfile")
else()
    target_link_libraries(libsupernova ${SNDFILE})
endif()

find_library(RT NAMES rt)
if (NOT RT)
    message(SEND_ERROR "Cannot find librt")
else()
    target_link_libraries(libsupernova ${RT})
endif()

target_link_libraries(libsupernova oscpack tlsf boost_thread boost_system boost_filesystem boost_program_options)
target_link_libraries(supernova libsupernova)

install(TARGETS supernova
        DESTINATION "bin"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

if(LTO)
    set_property(TARGET supernova libsupernova
                 APPEND
                 PROPERTY COMPILE_FLAGS "-flto -flto-report"
    )

    set_property(TARGET supernova libsupernova
                 APPEND
                 PROPERTY LINK_FLAGS "-flto -flto-report"
    )
endif()