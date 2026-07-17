set(plugin_tests
    c_api_test.c
    shared_lib_test.cpp
)

set(CMAKE_SHARED_MODULE_PREFIX "")
set(CMAKE_SHARED_MODULE_SUFFIX ".scx")

# let plugins find their dependencies in the same folder
set(CMAKE_INSTALL_RPATH $ORIGIN)

foreach(test_file ${plugin_tests})
    get_filename_component(test_name ${test_file} NAME_WE)

    add_library(${test_name} MODULE ${test_file})
    target_include_directories(
        ${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/include/common
        ${CMAKE_SOURCE_DIR}/include/plugin_interface
    )

    set_property(TARGET ${test_name} PROPERTY FOLDER Test/Plugin/C_API)
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
# export symbols!
set_target_properties(testlib PROPERTIES
  CXX_VISIBILITY_PRESET default
)
target_link_libraries(shared_lib_test PRIVATE testlib)
target_link_libraries(shared_lib_test_supernova PRIVATE testlib)
set_property(TARGET testlib PROPERTY FOLDER Test/Plugin/C_API)
  set_property(TARGET shared_lib_test_supernova PROPERTY FOLDER Test/Supernova)
