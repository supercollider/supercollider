add_library(nova-server STATIC
            server/buffer_manager.cpp
            server/dsp_context.cpp
            server/memory_pool.cpp
            server/node_graph.cpp
            server/server.cpp
)

target_link_libraries(nova-server oscpack tlsf boost_thread boost_system)

