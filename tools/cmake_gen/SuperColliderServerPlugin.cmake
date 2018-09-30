# Brian Heim
# 2018-08-26
# Functions for configuring SuperCollider server plugins

function(sc_check_sc_path path)
    if(NOT path)
        set(sc_path_default "../supercollider")
        message(WARNING "No SC_PATH specified, defaulting to '${sc_path_default}'.")
        set(path "${sc_path_default}")
    endif()

    get_filename_component(full_path "${path}" ABSOLUTE BASE_DIR "${CMAKE_SOURCE_DIR}")

    # check main paths
    if(NOT EXISTS "${full_path}/include/plugin_interface/SC_PlugIn.h")
        set(msg_end "\nPlease set SC_PATH to the root folder of the SuperCollider project relative to the folder containing this CMakeLists.txt file")
        message(FATAL_ERROR "Could not find SuperCollider3 headers at '${full_path}'.${msg_end}")
    endif()

    # check supernova paths
    if (SUPERNOVA)
        if (NOT EXISTS ${full_path}/external_libraries/nova-tt/CMakeLists.txt)
            message(FATAL_ERROR "The nova-tt submodule in the SuperCollider repository is missing (required for SuperNova plugins). This probably means you forgot to clone submodules. To fix this, run `git submodule update --init` from the root folder of the SuperCollider repository")
        endif()
    endif()

    set(SC_PATH ${full_path} PARENT_SCOPE)
endfunction()

function(sc_set_shared_module_prefix)
    if(APPLE OR WIN32)
        set(CMAKE_SHARED_MODULE_SUFFIX ".scx" PARENT_SCOPE)
    endif()
    set(CMAKE_SHARED_MODULE_PREFIX "" PARENT_SCOPE)
endfunction()

function(sc_add_server_plugin_properties target is_supernova)
    set_target_properties(${target} PROPERTIES CXX_VISIBILITY_PRESET hidden)

    target_include_directories(${target} PUBLIC
        ${SC_PATH}/include/plugin_interface
        ${SC_PATH}/include/common
        ${SC_PATH}/common
    )
    if(is_supernova)
        target_include_directories(${target} PUBLIC
            ${SC_PATH}/external_libraries/nova-tt
            ${SC_PATH}/external_libraries/boost
        )
    endif()

    list(APPEND all_sc_server_plugins ${target})
    set(all_sc_server_plugins ${all_sc_server_plugins} PARENT_SCOPE)
endfunction()

function(sc_add_server_plugin name cpp sc schelp)
    if(SCSYNTH)
        set(sy_name "${name}_scsynth")
        add_library(${sy_name} MODULE "${cpp}")
        install(TARGETS ${sy_name} LIBRARY DESTINATION ${name})
        sc_add_server_plugin_properties(${sy_name} FALSE)
        message(STATUS "Added server plugin target ${sy_name}")
    endif()

    if(SUPERNOVA)
        set(sn_name "${name}_supernova")
        add_library(${sn_name} MODULE "${cpp}")
        # install scsynth/supernova targets to same dir
        install(TARGETS ${sn_name} LIBRARY DESTINATION ${name}) 
        sc_add_server_plugin_properties(${sn_name} TRUE)
        message(STATUS "Added server plugin target ${sn_name}")
    endif()

    if(sc)
        install(FILES "${sc}" DESTINATION ${name}/Classes)
    endif()
    if(schelp)
        install(FILES "${schelp}" DESTINATION ${name}/Help)
    endif()
endfunction()

sc_set_shared_module_prefix()

set(all_sc_server_plugins)
