set(plugin_tests
    c_api_test.c
    shared_lib_test.cpp
)

set(CMAKE_SHARED_MODULE_PREFIX "")
if (APPLE OR WIN32)
    set(CMAKE_SHARED_MODULE_SUFFIX ".scx")
endif()

# let plugins find their dependencies in the same folder
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

foreach(test_file ${plugin_tests})
    get_filename_component(test_name ${test_file} NAME_WE)

    add_library(${test_name} MODULE ${test_file})
    target_include_directories(
        ${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/include/common
        ${CMAKE_SOURCE_DIR}/include/plugin_interface
    )

    # C plugins cannot be built for Supernova (yet)
    if (NOT ${test_name} STREQUAL c_api_test)
        add_library(${test_name}_supernova MODULE ${test_file})
        target_compile_definitions(${test_name}_supernova PRIVATE SUPERNOVA)
        target_include_directories(
            ${test_name}_supernova PRIVATE
            ${CMAKE_SOURCE_DIR}/include/common
            ${CMAKE_SOURCE_DIR}/include/plugin_interface
        )
    endif()
endforeach()

# DLL for shared_lib_test(_supernova)
add_library(testlib SHARED testlib.cpp)
target_link_libraries(shared_lib_test PRIVATE testlib)
target_link_libraries(shared_lib_test_supernova PRIVATE testlib)
