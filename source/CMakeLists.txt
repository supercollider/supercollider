add_library(libsupernova STATIC
            sc/sc_plugin_interface.cpp
            sc/sc_synth.cpp
            sc/sc_synth_prototype.cpp
            sc/sc_synthdef.cpp
            sc/sc_ugen_factory.cpp
            sc/supercollider/Source/server/Samp.cpp
            server/buffer_manager.cpp
            server/dsp_context.cpp
            server/memory_pool.cpp
            server/node_graph.cpp
            server/server.cpp
            server/sc_osc_handler.cpp
            server/server_args.cpp
)

add_executable(supernova server/main.cpp)
add_subdirectory(sc)

set_property(TARGET libsupernova supernova
             APPEND
             PROPERTY COMPILE_DEFINITIONS JACK_BACKEND SUPERNOVA)

if (UNIX)
    set_property(TARGET libsupernova supernova
                 APPEND
                 PROPERTY COMPILE_DEFINITIONS DLOPEN)
endif (UNIX)

target_link_libraries(libsupernova oscpack tlsf boost_thread boost_system boost_filesystem boost_program_options rt jack sndfile)
target_link_libraries(supernova libsupernova)

install(TARGETS supernova
        DESTINATION "bin"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
